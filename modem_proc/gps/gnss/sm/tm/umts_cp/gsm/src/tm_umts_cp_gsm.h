#ifndef TM_UMTS_CP_GSM_H
#define TM_UMTS_CP_GSM_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-CP-GSM Header File 

GENERAL DESCRIPTION

  Definition of variables used by tm_umts_cp_gsm.c and other modules within
  the TM-UMTS-CP-GSM submodule.

  Prototypes of functions defined in tm_umts_cp_gsm.c and used by multiple
  modules within the TM-UMTS-CP-GSM submodule. 
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.

Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/gsm/src/tm_umts_cp_gsm.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/31/15   mj      Changes to fix E911 on second SUB in G+G config
  04/02/15   mj      Add new type umts_cp_gsm_rr_as_id_info_s_type
  09/11/14   mj      Add e_session_as_id member to store GSM CP session's current asid
  08/18/09   atien   Add tm_umts_cp_gsm_stop_prm()
  09/19/08   LT      Buffering ACQ-assist. for injecting at right time.
  05/03/07   LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "tm_prtl_iface.h"
#include "tm_umts_common_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TM_UMTS_CP_GSM_TIMER_ID_RANGE     (TM_UMTS_CP_TIMER_ID_RANGE + 0x800000)
#define TM_UMTS_CP_GSM_CF_TIMER_ID        (TM_UMTS_CP_GSM_TIMER_ID_RANGE + 0)
#define TM_UMTS_CP_GSM_CF_MAX_MGP_ON_TIMER_ID   (TM_UMTS_CP_GSM_TIMER_ID_RANGE + 1)
#define TM_UMTS_CP_GSM_ACC_RESET_TIMER_ID (TM_UMTS_CP_GSM_TIMER_ID_RANGE + 2)


/*
 *  These are timeout values used mainly for GSM preemption, especially for but 
 *  not necessarily limited to, E911 call processing
 */
#define TM_UMTS_CP_GSM_MAX_MGP_ON_IN_MS  (40 * 1000)
#define TM_UMTS_CP_GSM_ACC_RESET_DYNAMIC_QOS_DISABLE_TIME_IN_MS   (24 * 1000)

typedef enum
{
  C_UMTS_CP_GSM_SESSION_NONE,
  C_UMTS_CP_GSM_SESSION_NI, /* network-initiated */
  C_UMTS_CP_GSM_SESSION_UI, /* UE-initiated immediate */
  C_UMTS_CP_GSM_SESSION_KEEP_WARM /* Keep Warm initiated session */
} umts_cp_gsm_callflow_kind_enum_type;


typedef enum
{
  C_UMTS_CP_GSM_AGPS_MODE_UNDEF,
  C_UMTS_CP_GSM_AGPS_MODE_UE_BASED,
  C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED,
  C_UMTS_CP_GSM_AGPS_MODE_CONV,
} umts_cp_gsm_session_agps_mode_enum_type;

typedef enum
{
  /* No Session Started Yet */
  C_UMTS_CP_GSM_CF_STATE_NONE,

  /* Session Started, but no RRLP requested yet */
  C_UMTS_CP_GSM_CF_STATE_STARTED,

  /* Waiting for RRLP assistance data from the network */
  C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP,

  /* Waiting for MGP to produce measurements and/or position */
  C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME,

  /* Waiting for the network to return a position (MA only) */
  C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT,
} umts_cp_gsm_cf_state_enum_type;

typedef struct
{
  uint8     u_gps_session_timeout_value_secs;
  uint32    q_hor_acc_meters;
} umts_cp_gsm_session_qos;

typedef struct
{
  uint32                          q_time_stamp_msec;
  rrlp_FixType                    e_fix_type;
  gad_ellip_alt_unc_ellip_type    z_pos;
} umts_cp_gsm_pending_pos_type;

typedef struct
{
  boolean   u_ref_time_week_num_valid;
  uint16    w_ref_time_week_num;
  boolean   u_pos_time_stamp_valid;
  uint32    q_pos_time_stamp;
} msa_pos_time_stamp_type;


typedef struct
{
  boolean   u_valid;
  tm_post_data_payload_type z_cached_svdir;
  tm_post_data_payload_type z_cached_steer;
} cached_acq_assist_s_type;

typedef struct
{
  boolean u_valid;
  sm_GpsMeasRptStructType  z_cached_gps_meas_rpt;
} cached_gps_meas_s_type;

typedef struct
{
  boolean v_rr_event_as_id_received;
  sys_modem_as_id_e_type e_current_rr_as_id;
  sys_modem_as_id_e_type e_cached_rr_as_id;
} umts_cp_gsm_rr_as_id_info_s_type;

/*
 *  This structure contains all "per-session" GSM data.  It is cleared
 *  at the end (or beginning) of each session.
 */
typedef struct
{
  /*
   *  PDAPI and TM session ID/Handle
   */
  pdsm_client_id_type                           q_pdapi_client_id;
  tm_sess_handle_type                           tm_active_session_handle; 

  /*
   *  This is a copy of the Start Session Request parameters that
   *  were used to start the session, assuming it is an MO session.
   *  This field will be left blank if this is an MT session.
   */
  prtl_start_sess_req_param_s_type              z_start_sess_req_param;

  /*
   *  Session QoS information
   */
  umts_cp_gsm_session_qos                       z_session_qos;

  /*
   *  GPS Assistance Data is encoded into Gsm0931 formatting and
   *  stored in here for use later.
   */
  GpsAssisData_Gsm0931                          z_GpsAssistData;
  
  /*
   *  Cached (pending) position stored when PDAPI position is returned
   *  to the GSM-CP protocol sub-module before an MPR is received.
   */
  umts_cp_gsm_pending_pos_type                  z_pending_pos;

  /*
   *  Assistance Data is stored here
   */
  tm_network_assist_s_type                      z_network_assist;

  /*
   *  Used for time-stamping MS-Assisted position
   */
  msa_pos_time_stamp_type                       z_msa_pos_time_stamp;

  /*
   *  NI vs. UI
   */
  umts_cp_gsm_callflow_kind_enum_type           e_callflow_kind;

  /*
   *  UE-BASED vs UE-ASSISTED
   */
  umts_cp_gsm_session_agps_mode_enum_type       e_agps_mode;

  /*
   *  This defines the various callflow states for the GSM-CP protocol
   *  sub-module.
   */
  umts_cp_gsm_cf_state_enum_type                e_cf_state;

  /*
   *  Either LOCATION_ESTIMATE or ASSISTANCE_DATA.  This is used to 
   *  define what type of MO callflow is used.
   */
  molr_type_e_type                              e_mo_lr_type;

  /*
   *  This is a bit mask describing the assistance data pieces that
   *  have been received this session.  It is used in MTMA mode to 
   *  determine what wishlist should be requested from the server.
   *  It is also used in both MTMA and MTMB to determine if an MPR
   *  has been received without assistance data.
   *  The bit mask is defined by the #defines in sm_api.h which all
   *  start with C_GPS_ASSIS_.  
   */
  uint32                                        q_agps_data_rcvd;

  /*
   *  This is a bit mask describing which almanac and ephemeris data
   *  has been received.  It is used to provide a list of non-existent
   *  or unhealthy SVs to MGP.
   */
  uint32                                        q_alm_sv_list;
  uint32                                        q_eph_sv_list;

  /*
   *  Set to TRUE when an MOLR-Invoke is sent out.  This is used later
   *  to prevent the code from sending out another MOLR-Invoke after
   *  a non-zero wishlist is received from MGP.
   */
  boolean                                       u_molr_invoke_sent;

  /*
   *  Set to TRUE when a PDAPI fix is reported from MGP.  This is
   *  used later in the callflow to determine if an existing fix can be
   *  returned to the network OR if the callflow state machine must
   *  wait for one to be reported.
   */
  boolean                                       u_pdapi_fix_reported;

  /*
   *  Set to TRUE when an MT callflow requests additional assistance
   *  data (ie. the wishlist) from the network.
   */
  boolean                                       u_wishlist_requested;

  /*
   *  Set to TRUE when a Measured Position Request is received from the 
   *  network.  This is used later in MT callflows to determine whether
   *  position needs to be returned to the network or cached.  Position
   *  will be cached if a PDAPI fix is reported from MGP
   *  before the MPR is received.
   */
  boolean                                       u_mpr_rcvd;

  /*
   *  Set to TRUE when the FINAL Measured Position Request is received.
   *  The FINAL Measured Position Request will be the first (and only) one
   *  in the callflow if our wishlist is empty.  If we request additional
   *  assistance data from the network (ie. the wishlist) then the FINAL
   *  Measured Position Request is the one that is sent after we request
   *  the additional assistance data.
   */
  boolean                                       u_final_mpr_rcvd;

  /*
   *  Set to TRUE when a session is started with TM-Core.
   */
  boolean                                       u_tm_core_session_started;

  /*
   *  Set to TRUE when ref location is to be acquired BY FORCE
   */
  boolean                                       u_force_ref_loc;

  /*
   *  Set to TRUE when a network session is in progress
   */
  boolean                                       u_network_session_on;

  /*
   *  Set to TRUE when accuracy reset timer expires
   */
  boolean                                       u_accuracy_reset_timer_expired;

  /*
   *  Set to TRUE when any RRLP message is received.  This flag is used
   *  during E911 calls to determine if a positioning session has been
   *  initiated before the MINIMUM_E911_CALL_TIMER (currently 32 seconds)
   *  expires.
   */
  boolean                                       u_rrlp_rcvd;

  /*
   *  The u_preemption_flag is set when a GSM preemption event is
   *  received.  It is never cleared until the end of the session.  This
   *  flag can be used to alter logic which is dependent on whether or
   *  not there has been at least one preemption in the current session.
   */
  boolean                                       u_preemption_flag;
  
  
  /* Cached aquisition assistance data */
  cached_acq_assist_s_type                     z_cached_acq_assist;

  /* Cached GPS measurement */
  cached_gps_meas_s_type											 z_cached_gps_meas;

  /* Caches current CP-GSM session as_id from RR */
  umts_cp_gsm_rr_as_id_info_s_type			  z_rr_as_id_info;
  
  
} umts_cp_gsm_session_status_s_type;


/*
 *  This structure contains all "global" data used by the GSM module
 *  which is not "per-session".  It is not cleared at the end (or beginning)
 *  of a session.
 */
typedef struct
{
  /*
   *  This timer is used for everything except max_mgp_on and acc_reset timers.
   */
  os_TimerMsgType                     *p_gsm_timer;

  /*
   *  This timer is used for timing the length of max. MGP_ON timer.
   *  The main use for this timer is to shut off MGP if no RRLP session
   *  is started after the E911 call is brought up.  
   */
  os_TimerMsgType                     *p_max_mgp_on_timer;

  /*
   *  This timer is used to measure the amound of time elapsed since the 
   *  beginning of the E911 call and its expiration has impact on the use
   *  of dynamically changing QOS accuracy values and the deactivation of  
   *  "dynamic QOS" in MGP as the end of the call is approached.
   */
  os_TimerMsgType                     *p_acc_reset_timer;

  /*
   *  Set to TRUE when an E911 call is originated.  Cleared at the end
   *  of the session.  This flag will be used to make sure that the FIRST
   *  positioning session of an E911 call is treated as an E911 callflow.
   *  This means that MGP will be turned on when the call is brought up,
   *  the e911_timer will get used, and rules about not asking for certain
   *  types of AD (like almanac) will apply.
   */
  boolean                             u_e911_in_progress;

  /* This is used to keep track of whether the current RRLP session
     is the first one since E911 call is up; it is needed for the rules
     of multiple NILR in E911. Initialized to 0 (FALSE). Reset to 0 everytime
     an E911 call is established and dropped. Set to TRUE when the first RRLP
     session is over. */
  boolean                             u_e911_subsequent_rrlp_sess;

} umts_cp_gsm_s_type;


#ifdef UMTS_CP_GSM_MAIN_VARIABLES_DEFINED
#define extern
#endif

/*
 *  This variable contains all of the per-session "global" data/state
 */
extern umts_cp_gsm_session_status_s_type   z_umts_cp_gsm_session_status;

/*
 *  This variable contains all "global" data used by the GSM module
 *  which is not "per-session".  It persists through sessions.
 */
extern umts_cp_gsm_s_type                  z_umts_cp_gsm_data;

#ifdef extern
#undef extern
#endif

/*
 *  Module Initialization Functions
 */
void    tm_umts_cp_gsm_module_init
(
              void
); 
void    tm_umts_cp_gsm_session_init
(
              void
); 
boolean tm_umts_cp_gsm_pdapi_mapping 
(
              const prtl_start_sess_req_param_s_type *start_sess_req_param
);
void    tm_umts_cp_gsm_inform_core_nt_assist_end
(
              void
);
void    tm_umts_cp_gsm_set_cf_state
(
              umts_cp_gsm_cf_state_enum_type e_cf_state
);
void    tm_umts_cp_gsm_callflow_disrupt_handler
(
              void
);
void    tm_umts_cp_gsm_inform_core_stop_session
(
              tm_sess_stop_reason_e_type    stop_reason
);
void    tm_umts_cp_gsm_handle_lr_data
(
              sm_GnssFixRptStructType* lr_data_ptr
);
void    tm_umts_cp_gsm_handle_sess_info_status
(
              tm_aiding_data_wishlist_s_type  *p_wishlist
);
void    tm_umts_cp_gsm_handle_lcs_event
(
              prtl_lcs_event_s_type       *p_event 
);
void    tm_umts_cp_gsm_handle_phone_event
(
              prtl_phone_event_s_type     *p_event 
);
void    tm_umts_cp_gsm_handle_rr_event
(
              const prtl_rr_event_s_type  *p_event 
);
void    tm_umts_cp_gsm_handle_mm_event
(
              const prtl_mm_event_s_type  *p_event 
);
uint16  tm_umts_cp_gsm_write_data
(  
              const byte  *p_data, 
              uint16      length 
);
void    tm_umts_cp_gsm_callflow_kind_set
(
              umts_cp_gsm_callflow_kind_enum_type   u_callflow_kind
);
boolean tm_umts_cp_gsm_tx_rrlp_mpr_err
( 
              rrlp_LocErrorReason       e_error_reason 
);
void    tm_umts_cp_gsm_request_wishlist
( 
              void 
);
void    tm_umts_cp_gsm_cancel_cf_timer
(
              void
);
boolean tm_umts_cp_gsm_start_sess_req
(
              umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode
);
void    tm_umts_cp_gsm_handle_max_mgp_on_timer_cb
( 
              void 
);
void tm_umts_cp_gsm_handle_acc_reset_timer_cb
(
              void
);
void    tm_umts_cp_gsm_handle_cf_timer_cb
( 
              void 
);
void tm_umts_cp_gsm_start_max_mgp_on_timer
(            
              uint32  q_time_ms 
);
void tm_umts_cp_gsm_cancel_max_mgp_on_timer
(              
              void
);
void tm_umts_cp_gsm_start_acc_reset_timer( 
             uint32  q_time_ms
);
void tm_umts_cp_gsm_cancel_acc_reset_timer
( 
              void
);
void    tm_umts_cp_gsm_send_continue_session_to_tm
(             uint8 u_acc_thresh,
              uint8 u_timeout_secs,
              boolean u_need_wishlist,
              boolean u_use_dynamic_qos
);

boolean tm_umts_cp_gsm_stop_prm
(
              void
);

#ifdef __cplusplus
}
#endif

#endif  /* TM_UMTS_CP_SS_H */
