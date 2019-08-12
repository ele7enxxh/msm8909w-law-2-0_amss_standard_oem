#ifndef TM_UMTS_CP_WCDMA_H
#define TM_UMTS_CP_WCDMA_H

/*===========================================================================

                            TM_UMTS_CP_WCDMA Header File

DESCRIPTION

  Definition of variables used by tm_umts_cp_wcdma.c and other modules within
  the TM-UMTS-CP-WCDMA submodule.

  Prototypes of functions defined in tm_umts_cp_wcdma.c and used by multiple
  modules within the TM-UMTS-CP-WCDMA submodule.


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Confidential and Proprietary - Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/wcdma/inc/tm_umts_cp_wcdma.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  06/09/15    mj     Add support for W+W feature
  03/14/15    rh      Added support for MultiSIM NV
  06/30/09    atien   1. add new function parameter to pass cf timer value in
                         tm_umts_cp_wcdma_set_cf_state
                      2. add CP_WCDMA_CF_TIMER_MOLR_INVOKE_DEFAULT - default timer
                         value for waiting MC after sending MOLR-Invoke
  08/02/07    LT      Handling SVs non-exist.
  05/03/07    LT      Initial version.

===========================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/wcdma/inc/tm_umts_cp_wcdma.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "gps_variation.h"
#include "sm_api.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define CP_WCDMA_CF_TIMER_WAIT_MC_DEFAULT      (10)                  /* call flow timer value for waiting MC from network [sec] */
#define TM_UMTS_CP_WCDMA_MAX_MGP_ON_IN_MS      (40 * 1000)           /*max mgp on timer value for waiting for positioning session*/


#define CP_WCDMA_CF_TIMER_ID             TM_UMTS_CP_TIMER_ID_RANGE   /* call flow timer */
#define CP_WCDMA_ALM_REQ_HD_TIMER_ID     (CP_WCDMA_CF_TIMER_ID + 1)  /* alm. req. hold-off timer */
#define CP_WCDMA_EPH_REQ_HD_TIMER_ID     (CP_WCDMA_CF_TIMER_ID + 2)  /* eph. req. hold-off timer */
#define CP_WCDMA_IONO_REQ_HD_TIMER_ID    (CP_WCDMA_CF_TIMER_ID + 3)  /* IONO. req. hold-off timer */
#define CP_WCDMA_UTC_REQ_HD_TIMER_ID     (CP_WCDMA_CF_TIMER_ID + 4)  /* UTC req. hold-off timer */
#define CP_WCDMA_REFLOC_REQ_HD_TIMER_ID  (CP_WCDMA_CF_TIMER_ID + 5)  /* ref. loc. req. hold-off timer */
/*MAX MGP on timer is used to protect against scenario where no positioning session
 is started after originating an E911 call. When this timer expires, the session is ended
 thereby stopping the MGP*/
#define CP_WCDMA_MAX_MGP_ON_TIMER_ID     (CP_WCDMA_CF_TIMER_ID + 6)  /* max mgp on timer */

#ifdef FEATURE_GANSS_SUPPORT
/* Glonass timers */
  #define CP_WCDMA_GLO_EPH_REQ_HD_TIMER_ID            (CP_WCDMA_CF_TIMER_ID + 7)  /* Glonass eph. req. hold-off timer */
  #define CP_WCDMA_GLO_ALM_REQ_HD_TIMER_ID            (CP_WCDMA_CF_TIMER_ID + 8)  /* Glonass eph. req. hold-off timer */
  #define CP_WCDMA_GLO_AUXI_INFO_REQ_HD_TIMER_ID      (CP_WCDMA_CF_TIMER_ID + 9)  /* alm. req. hold-off timer */
  #define CP_WCDMA_GLO_TIME_MODEL_REQ_HD_TIMER_ID     (CP_WCDMA_CF_TIMER_ID + 10)  /* alm. req. hold-off timer */
#endif


/* if more than one timer ID defined, "OR (|) them" to form the mask */
#ifdef FEATURE_GANSS_SUPPORT
  #define TM_UMTS_CP_WCDMA_TIMER_DISPATCH_MASK  ( CP_WCDMA_CF_TIMER_ID | CP_WCDMA_ALM_REQ_HD_TIMER_ID | \
    CP_WCDMA_EPH_REQ_HD_TIMER_ID | CP_WCDMA_IONO_REQ_HD_TIMER_ID | CP_WCDMA_UTC_REQ_HD_TIMER_ID | \
    CP_WCDMA_REFLOC_REQ_HD_TIMER_ID | CP_WCDMA_MAX_MGP_ON_TIMER_ID | CP_WCDMA_GLO_EPH_REQ_HD_TIMER_ID | \
    CP_WCDMA_GLO_ALM_REQ_HD_TIMER_ID | CP_WCDMA_GLO_AUXI_INFO_REQ_HD_TIMER_ID | CP_WCDMA_GLO_TIME_MODEL_REQ_HD_TIMER_ID)
#else
  #define TM_UMTS_CP_WCDMA_TIMER_DISPATCH_MASK  ( CP_WCDMA_CF_TIMER_ID | CP_WCDMA_ALM_REQ_HD_TIMER_ID | \
  CP_WCDMA_EPH_REQ_HD_TIMER_ID | CP_WCDMA_IONO_REQ_HD_TIMER_ID | CP_WCDMA_UTC_REQ_HD_TIMER_ID | \
  CP_WCDMA_REFLOC_REQ_HD_TIMER_ID  | CP_WCDMA_MAX_MGP_ON_TIMER_ID)
#endif

/* Max Session Timeout for multi-periodic reporting */
#define TM_MULTI_REPT_MAX_SESSION_TIMEOUT (2* 60 * 60) /* 2 Hrs: test case on ULTS expects minimum of 900 secs of reporting*/

#define PERMIT_QOS_TIME_RESET    TRUE
#define FORBID_QOS_TIME_RESET    FALSE

/* used by nv_item aagps_3g_mo_lrsupport */
#define AAGPS_NV_3G_MO_LR_LOW_ACCURACY 0x01
#define AAGPS_NV_3G_MO_LR_HIGH_ACCURACY 0x02
#define AAGPS_NV_3G_MO_LR_ASSIST_DATA  0x04  /* demand using AssistanceData call flow */

#define GANSS_POSITIONING_METHOD_GPS_BMASK      0x8000  /* Bit 0 - Enable/disable GPS */
#define GANSS_POSITIONING_METHOD_GLONASS_BMASK  0x0400  /* Bit 5 - Enable/disable GLONASS */


/*
*  Possible position method & position method types.
*  First 4 LSB  bits (bit 0 to 3 from right) represents "UEB, UEA, Standalone, reserved" position method type
*  Next 4 bits (bit 4 to 7 from right) represents "GPS, GLO, reserved, reserved" position method
*/
#define WCDMA_POSITION_METHOD_TYPE_UE_BASED         0x01    /* UE-B position method type */
#define WCDMA_POSITION_METHOD_TYPE_UE_ASSISTED      0x02    /* UE-A position method type */
#define WCDMA_POSITION_METHOD_TYPE_UE_STANDALONE    0x04    /* UE Standalone position method type */
#define WCDMA_POSITION_METHOD_A_GPS            0x10    /* GPS position method */
#define WCDMA_POSITION_METHOD_A_GLO            0x20    /* GLONASS position method */

/* Mask position method & type bits */
#define WCDMA_POSITION_METHOD_TYPE_BMASK        0x0F    /* Select all possbile position method type (UE-A, UE-B, Standalone) bits */
#define WCDMA_POSITION_METHOD_BMASK             0xF0    /* Select all possbile position method (GPS, GLO) bits */

/*
*  Valid combination of A-GNSS position modes derived from above mentioned possibilities
*/
typedef enum
{
  eWCDMA_POSITION_MODE_AGNSS_UNDEF = 0,                                                                /* None */
  eWCDMA_POSITION_MODE_AGPS_UE_BASED = (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_TYPE_UE_BASED),       /* GPS UE-B position method */
  eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED = (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_TYPE_UE_ASSISTED),    /* GPS UE-A position method */
  eWCDMA_POSITION_MODE_AGPS_CONV = (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_TYPE_UE_STANDALONE),           /* GPS Standalone */
  eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED = (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_A_GLO | WCDMA_POSITION_METHOD_TYPE_UE_BASED),     /* GPS & GLONASS UE-B position method */
  eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED = (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_A_GLO | WCDMA_POSITION_METHOD_TYPE_UE_ASSISTED),  /* GPS & GLONASS UE-A position method */
} wcdma_position_mode_e_type;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  C_UMTS_CP_WCDMA_SESSION_NONE,
  C_UMTS_CP_WCDMA_SESSION_NI, /* network-initiated */
  C_UMTS_CP_WCDMA_SESSION_UI, /* UE-initiated immediate */
  C_UMTS_CP_WCDMA_SESSION_KEEP_WARM /* Keep Warm initiated session */
} umts_cp_wcdma_callflow_kind_enum_type;

typedef enum
{
  C_UMTS_CP_WCDMA_CF_STATE_NONE,
  C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC = 0x0001,
  C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT = 0x0002, /* Wait time for MGP to return with measurement or location estimate. reportIntervalLong */
  C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT = 0x0004,
  C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME = 0x0008
} umts_cp_wcdma_cf_state_enum_type;

typedef enum /* UEP MeasuredResult Sequence enum., used in forming RRC-measurementReport */
{
  C_RRC_MSR_REPORT_POS,
  C_RRC_MSR_REPORT_MEAS,
  C_RRC_MSR_REPORT_LOCERR
} umts_cp_wcdma_cf_rrc_uep_meas_res_seq_enum_type;

typedef struct
{
  uint8 u_gps_session_timeout_value_secs;
  uint32 q_hor_acc_meters;
} umts_cp_wcdma_session_qos;



#ifdef FEATURE_GANSS_SUPPORT
typedef struct
{
  boolean u_freq_num_valid;
  glo_EphStructType z_eph_glo_data;
}rrc_glo_eph_buffer_s_type;
#endif /* FEATURE_GANSS_SUPPORT */

#ifdef FEATURE_CGPS_DUAL_WCDMA
typedef struct
{
  boolean u_rrc_event_as_id_received;
  sys_modem_as_id_e_type e_current_rrc_as_id;
  sys_modem_as_id_e_type e_cached_rrc_as_id;
} umts_cp_wcdma_rrc_as_id_info_s_type;
#endif

typedef struct
{
  pdsm_client_id_type q_pdapi_client_id;
  tm_sess_handle_type tm_active_session_handle;
  umts_cp_wcdma_callflow_kind_enum_type u_callflow_kind;  /* NI, UI */
  wcdma_position_mode_e_type e_gnssPosMode;      /* GPS/GLO UE-Based, UE-Assisted, ... */
  boolean u_network_session_on;
  umts_cp_wcdma_session_qos z_session_qos;
  GpsAssisData_Gsm0931 z_GpsAssistData;  /* must include tm_umts_common_utils. h */
  umts_cp_wcdma_cf_state_enum_type e_cf_state;
  boolean u_pdapi_fix_reported;

  uint32 q_time_stamp_pos_pending_msec; /* time stamp of position pending to network */
  gad_ellip_alt_unc_ellip_type z_pending_pos_to_network; /* used when a position is reported to pdapi prior to completion
                                                               of the network call flow */

  molr_type_e_type mo_lr_type; /* when zeroed: LOCATION_ESTIMATE */
  boolean additional_assist_request_done; /* TRUE: Requested assistance data status to TM */
  uint16 w_rrc_reporting_interval_long;  /* RRC measurementControl's repsonse time in sec. */
  boolean tm_core_session_started;        /* FALSE: TM session has not started */

  uint16 w_cp_periodic_reports_remaining; /* number of remaining reports in a multi-reporting periodic session */
  uint16 w_cp_periodic_reports_total_num; /* total number of reports in a multiple-reporting periodic session */
  uint32 q_cp_periodic_reporting_interval_sec;
  uint32 q_stored_cp_periodic_reporting_interval_sec; /* used to restore to the original value when getting out of PCH */
  uint16 w_lm_report_count; /* count how many fix or no-fix reports before the 1st or since last RRC reporting */

  boolean u_pdapi_comm_begin_sent;
  boolean u_pdapi_comm_done_sent;

  boolean rrc_msr_report_timeout; /* if TRUE, timeout has occured */

  gnss_time_info z_set_assist_pos_time_stamp; /* use for time-stamping UE-Assisted position */

  boolean allow_rrc;

  boolean u_force_request_ref_location;

  boolean qos_time_started; /* LM has been given the true qos_time */

  uint32 q_assist_delivered_list;       /* GPS assistance data delivered to SM list */
  uint32 q_alm_delivered_sv_list;       /* bit map of alm. delivered; SV1 on Bit 0 */
  uint32 q_eph_delivered_sv_list;       /* bit map of eph. delivered; SV1 on bit 0 */

  uint32 q_glo_assists_delivered_list;  /* Glonass assistance data delivered to SM list */
  #ifdef FEATURE_GANSS_SUPPORT
  uint32 q_glo_alm_delivered_sv_list;   /* Bit map of Glonass alm. delivered; SV1 on Bit 0 */
  uint32 q_glo_eph_delivered_sv_list;   /* Bitmap of Glo eph. delivered; SV1 on bit 0 */
  #endif

  #ifdef FEATURE_EXTENDED_RESP_TIME
  uint8 u_extended_ni_resp_time_sec; /* used in NI call flows, the intended max. session time */
  uint32 q_additional_req_margin_sec; /* number of seconds before RRC ReportingIntervalLong expiration
                                         when UE sends additional assistance request aking for IONO */
  #endif

  /* Cached GNSS measurement */
  SmGnssMeasReportStructType z_cached_gnss_meas;

  /* indicator of appBased tracking session */
  boolean u_appbased_tracking;

  #ifdef FEATURE_CONTINUE_MTLR_SESSION
  /* Flag is set when MT-LR session is in progress */
  boolean u_mtlr_in_progress;
  #endif

  /* Encoded GNSS measurement report status (valid or invalid). Use SM_GNSS_MEAS_REPORTS_BMAP_XXX macro  */
  uint8 u_GnssMeasEncResultBmap;
  gps_TimeStructType z_GpsTime4GloMeas;                   /* Store GPS measurement clock time from the Glonass measurement block */

  #ifdef FEATURE_GANSS_SUPPORT
  uint16 w_GanssPosMethods;                                /* See GANSS_POSITIONING_METHOD_xxx bitmask macro definition. */
  prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV]; /* Combined buffer for Glonass eph. data with freq. number
                                                        from aux. info assistance */
  prot_glo_acqast_buffer_s_type z_prot_glo_acqasst_buf;   /* Buffer to combine Glonass Acq. Assistance data with the freq. number in aux. info assistance */
  prot_glo_acqasst_sat_buffer_s_type z_prot_glo_acqasst_subset;  /* Store subset of Glonass Acq. Assistance parameters when other dependent ADs aren't available */
  prot_glo_svcn_buffer_s_type z_ProtGloSvCnBuf;        /* Store Glo Auxi Info when GLO AcqAssist AD isn't at the time of GloAuxiInfo processing */

  #endif /* FEATURE_GANSS_SUPPORT */

  #ifdef FEATURE_CGPS_DUAL_WCDMA
  /* Caches current CP-WCDMA session as_id from RRC */
  umts_cp_wcdma_rrc_as_id_info_s_type			  z_rrc_as_id_info;
  #endif

} umts_cp_wcdma_session_status_s_type;


typedef struct
{
  uint8 u_Saved;                              /* Decide whether to save MRM */
  uint8 u_MeasId;
  rrc_meas_rpt_struct_type *p_SavedMessBuff;  /* Save Measurement Report Message (MRM) */
} rrc_saved_meas_params_s_type;

typedef struct
{
  rrc_meas_params_enum_type u_rrc_param_type;    /* type of RRC command: measControl, assistaData, ... */
  uint8 u_rrc_meas_cmd_type; /* measureCommand type: setUp, modify, ... */
  boolean u_conf_ind;          /* if true, confirmation needed */
  uint8 u_ack_num;           /* the number for confirmation */
  uint8 u_meas_id;
  uint8 u_report_mode;
  boolean rrc_periodic_multi_report; /* if TRUE, RRC periodic reporting with N>1 */
  uint8 u_transfer_mode;
  rrc_saved_meas_params_s_type z_saved_meas;
} rrc_params_type;




#ifdef UMTS_CP_WCDMA_MAIN_VARIABLES_DEFINED
  #define extern
#endif
/*******  internal data *********************************************/
extern umts_cp_wcdma_session_status_s_type z_umts_cp_wcdma_session_status;
extern os_TimerMsgType *umts_cp_wcdma_cf_timer;
extern pdsm_pd_info_s_type z_umts_cp_wcdma_pd_info_buffer;
extern rrc_params_type z_rrc_params;
extern rrc_meas_rpt_struct_type z_measrpt;

/* GPS holdoff timers */
extern os_TimerMsgType *cp_wcdma_alm_req_holdoff_timer;
extern os_TimerMsgType *cp_wcdma_eph_req_holdoff_timer;
extern os_TimerMsgType *cp_wcdma_iono_req_holdoff_timer;    /* Same timer for GPS & GANSS Iono assistance data, since
                                                                either one is inject. */
extern os_TimerMsgType *cp_wcdma_utc_req_holdoff_timer;     /* GPS UTCModel holdoff timer. */
extern os_TimerMsgType *cp_wcdma_refloc_req_holdoff_timer;  /* GPS & GANSS reference location are of same type. So
                                                               one timer is enough to gaurd the refloc */

#ifdef FEATURE_GANSS_SUPPORT
/* Glonass holdoff timers */
extern os_TimerMsgType *cp_wcdma_glo_eph_req_holdoff_timer;
extern os_TimerMsgType *cp_wcdma_glo_alm_req_holdoff_timer;
extern os_TimerMsgType *cp_wcdma_glo_auxi_info_req_holdoff_timer;
extern os_TimerMsgType *cp_wcdma_glo_time_model_req_holdoff_timer;
#endif

/*MAX MGP on timer is used to protect against scenario where no positioning session
 is started after originating an E911 call. When this timer expires, the session is ended
 thereby stopping the MGP*/
extern os_TimerMsgType *umts_cp_wcdma_max_mgp_on_timer;


#ifdef extern
  #undef extern
#endif

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

boolean tm_umts_cp_wcdma_pdapi_mapping( prtl_start_sess_req_param_s_type *start_sess_req_param );
void tm_umts_cp_wcdma_lcs_loc_notif_ind_proc( tm_cm_sups_data_s_type *p_sups_data );
void tm_umts_cp_wcdma_inform_core_nt_assist_end( boolean u_permit_qostime_reset );
void tm_umts_cp_wcdma_set_cf_state( umts_cp_wcdma_cf_state_enum_type e_cf_state, boolean u_cf_molor_invoke_timer );
void tm_umts_cp_wcdma_session_cleanup( void );
void tm_umts_cp_wcdma_callflow_disrupt_handler( void );
void tm_umts_cp_wcdma_terminate_timer_cancel_cf_state( umts_cp_wcdma_cf_state_enum_type e_timer_state_to_terminate );
void tm_umts_cp_wcdma_lcs_molr_conf_proc( tm_cm_sups_data_s_type *p_sups_data );
void tm_umts_cp_wcdma_rrc_proc( prtl_rrc_event_s_type *p_rrc_event );
void tm_umts_cp_wcdma_mm_proc( prtl_mm_event_s_type *p_mm_event );
void tm_umts_cp_wcdma_pa_proc( prtl_pa_event_s_type *p_pa_event );
void tm_umts_cp_wcdma_handle_lr_data( sm_GnssFixRptStructType *lr_data_ptr, boolean u_is_final );
void tm_umts_cp_wcdma_handle_rpt_refpos( void );
void tm_umts_cp_wcdma_handle_prm_data( sm_GpsMeasRptStructType *p_prm_data );
void tm_umts_cp_wcdma_handle_nofix( void );
boolean tm_umts_cp_wcdma_encode_rrc_msrReport_pos( gad_ellip_alt_unc_ellip_type *p_ellipse,
                                                   uint32 q_toa_pos_ms,
                                                   sm_GpsFixRptStructType const *const lr_data_ptr );
boolean tm_umts_cp_wcdma_encode_rrc_msrReport_err( rrc_positioning_error_cause e_err_reason,
                                                   sm_InternalAssistDataStatusStructType const *const p_assist_data_status );
void tm_umts_cp_wcdma_inform_core_stop_session( tm_sess_stop_reason_e_type stop_reason );

void tm_umts_cp_wcdma_notif_verif_resp_proc( pdsm_lcs_notification_resp_s_type *p_user_resp );
void tm_umts_cp_wcdma_callflow_kind_set( umts_cp_wcdma_callflow_kind_enum_type u_callflow_kind );

/********************************************************************
*
* tm_umts_cp_wcdma_msb_assist_req_adj
*
* Function description:
*   Remove from the wishlist items already delivered by the network
*   in the current MSB session, and if after the adjustment there will
*   still something to request, always make sure ref. time is also
*   requested.
*   For A-GPS MSB only call flow ignore Glonass AD wishlist from MGP.
*
* Parameters:
*   Pointer to the wishlist to be checked and/or modified.
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_msb_assist_req_adj( uint32 *p_wishlist, uint32 *p_GloWishlist, const wcdma_position_mode_e_type e_GnssPosMode );

boolean tm_umts_cp_wcdma_report_ref_loc_to_pdapi_if_avail( void );
boolean tm_umts_cp_wcdma_rrc_send_cached_gps_meas( void );

/********************************************************************
*
* tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session
*
* Function description:
*   Send RRC measurementReport with cached GPS and/or GLONASS measurement if available
*   & end the session.
*
* Parameters:
*  None
*
* Return:
*  True if atleast (GPS and/or GLO) measurement is sent out, else return false.
*
*********************************************************************
*/

boolean tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session( void );

#ifdef FEATURE_EXTENDED_RESP_TIME
void tm_umts_cp_wcdma_extended_reporting_interval( uint8 *p_extended_ni_rep_time,
                                                   uint32 *p_additional_req_margin_sec );

void tm_umts_cp_wcdma_req_extend_resp_time( void );
#endif

void tm_umts_cp_wcdma_start_max_mgp_on_timer( uint32 q_time_ms );
void tm_umts_cp_wcdma_cancel_max_mgp_on_timer( void );

/*===========================================================================
FUNCTION tm_umts_cp_wcdma_post_non_exist_gnss_svs

DESCRIPTION
  Post/inject the GPS & GLONASS non-exist SVs(not to confuse with
  GLONASS non-exist frequency).

PARAMETERS
  p_umts_cp_wcdma_session[IN] : CP WCDMA session pointer

RETURN VALUE : none

SIDE EFFECTS : none

===========================================================================*/

void tm_umts_cp_wcdma_post_non_exist_gnss_svs( umts_cp_wcdma_session_status_s_type const *p_umts_cp_wcdma_session );

#ifdef FEATURE_GANSS_SUPPORT
/*===========================================================================
FUNCTION tm_umts_cp_wcdma_a_glo_rrc_cp_enabled

DESCRIPTION
  Check if RRC CP Glonass is enabled from the NV item.

DEPENDENCIES
  Depends on correct population of z_umts_nv_item_ram_copy struct.

RETURN VALUE
  True if RRC CP Glonass is enabled.

SIDE EFFECTS

===========================================================================*/
static GNSS_INLINE boolean tm_umts_cp_wcdma_a_glo_rrc_cp_enabled( void )
{
  if ( TM_CORE_CONFIG_DD(asst_glonass_pos_protocol) & PDSM_PA_ASST_GLO_PROTOCOL_RRC_CP )
  {
    return ( TRUE );
  }
  return ( FALSE );
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* TM_UMTS_CP_WCDMA_H */

