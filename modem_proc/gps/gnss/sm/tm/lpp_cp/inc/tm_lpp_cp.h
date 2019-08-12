#ifndef _TM_LPP_CP_H
#define _TM_LPP_CP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Utility

GENERAL DESCRIPTION
  This file contains TM's LPP protocol utilities, which convert decoded LPP
  messages for the consumption of TmCore.  

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
 
Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_cp/inc/tm_lpp_cp.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  03/29/16    kpn     Use of subscription ID while handling NAS DL message for LPP CP
  03/14/15    rh      Added support for MultiSIM NV  
  10/27/14    rk      Added LPP RSTD >24 measurement processing support
  09/02/14    gk      cache the tx_mgr_init as a static
  05/04/13    gk      Wait for wishlist resp before sending Req AD 
  05/14/13    rk      Removed duplicate gnss time structure. Instead use from tm_common.h
  03/26/13    rk      Resolved Ref Time IE not sent in CP LPP MSB Call flow issue.
===========================================================================*/

#include "gnss_lpp_ecid.h"
#include "mgp_api.h"               /* cgps_CellInfoCachedStruct */
#include "tm_prtl_iface.h"         /* include lte_ml1_ext_api.h for lte_ml1_gps_mrl_cell_meas_update_t */
#include "tm_lpp_tx_mgr.h"         /* tm_lpp_provide_cap_s_type */
#include "sm_api.h"                /* sm_InternalAssistDataStatusStructType */
#include "tm_umts_common_utils.h"  /* for gad_ellip_alt_unc_ellip_type */


/* Min assistance data needed by the engine for UEBased and UEAssited GNSS */
#define LPP_CP_UE_BASED_GPS_MIN_AD          ( C_GNSS_ASSIST_NAVMODL     | \
                                              C_GNSS_ASSIST_REFLOC      | \
                                              C_GNSS_ASSIST_REFTIME     )

#define LPP_CP_UE_BASED_GLO_MIN_AD          ( C_GNSS_ASSIST_NAVMODL     | \
                                              C_GNSS_ASSIST_REFLOC      | \
                                              C_GNSS_ASSIST_TIME_MODELS | \
                                              C_GNSS_ASSIST_AUXI_INFO   )


#define LPP_CP_UE_ASSISTED_GPS_MIN_AD       ( C_GNSS_ASSIST_ACQ         | \
                                              C_GNSS_ASSIST_REFTIME     )

#define LPP_CP_UE_ASSISTED_GLO_MIN_AD       ( C_GNSS_ASSIST_ACQ         | \
                                              C_GNSS_ASSIST_AUXI_INFO   | \
                                              C_GNSS_ASSIST_TIME_MODELS)


/* Max Session Timeout for multi-periodic reporting */
#define C_LPP_CP_TM_MULTI_REPT_MAX_SESSION_TIMEOUT (900) /* 900 seconds: test case on ULTS expects minimum of 900 secs of reporting*/

#define  C_LPP_CP_AGPS_MODE_UNDEF       (C_LPP_AGPS_MODE_UNDEF)
#define  C_LPP_CP_AGPS_MODE_UE_BASED    (C_LPP_AGPS_MODE_UE_BASED)
#define  C_LPP_CP_AGPS_MODE_UE_ASSISTED (C_LPP_AGPS_MODE_UE_ASSISTED)
#define  C_LPP_CP_AGPS_NO_POSITION      (C_LPP_AGPS_NO_POSITION)
#define  C_LPP_CP_AGPS_MODE_AUTO        (C_LPP_AGPS_MODE_AUTO)
#define  C_LPP_CP_MODE_UE_OTDOA         (C_LPP_MODE_UE_OTDOA)
#define  C_LPP_CP_MODE_UE_ECID          (C_LPP_MODE_UE_ECID)


typedef uint8 tm_lpp_cp_session_agps_mode_enum_type;

#define C_LPP_CP_AGNSS_SESS_GPS     (1 << GNSS_ID_Bitmap_gnss_ids_gps)
#define C_LPP_CP_AGNSS_SESS_SBAS    (1 << GNSS_ID_Bitmap_gnss_ids_sbas)
#define C_LPP_CP_AGNSS_SESS_QZSS    (1 << GNSS_ID_Bitmap_gnss_ids_qzss)
#define C_LPP_CP_AGNSS_SESS_GALILEO (1 << GNSS_ID_Bitmap_gnss_ids_galileo)
#define C_LPP_CP_AGNSS_SESS_GLONASS (1 << GNSS_ID_Bitmap_gnss_ids_glonass)
typedef uint32 tm_lpp_cp_session_agps_gnss_id;

typedef struct
{
   uint32    q_gps_session_timeout_value_secs;
   uint32    q_gps_session_timeout_value_msecs;
   uint32    q_hor_acc_meters;
} tm_lpp_cp_session_qos_type;

/* LPP CP Timers */
#define LPP_CP_CF_TIMER_ID      TM_LPP_CP_TIMER_ID_RANGE
#define LPP_CP_OTDOA_TIMER_ID  (LPP_CP_CF_TIMER_ID+1)
#define LPP_CP_NAS_SESS_TIMER  (LPP_CP_CF_TIMER_ID+2)
#define LPP_CP_ECID_TIMER_ID   (LPP_CP_CF_TIMER_ID+4)
#define LPP_CP_AD_TIMER_ID     (LPP_CP_CF_TIMER_ID+5)
#define LPP_CP_OTDOA_MUL_MEAS_TIMER_ID     (LPP_CP_CF_TIMER_ID+6)



typedef enum
{
  /* No Session Started Yet */
  C_LPP_CP_CF_STATE_NONE,

  /* Session Started, but no requested yet */
  C_LPP_CP_CF_STATE_STARTED               = 0x0001,

  /* Waiting for MGP to produce measurements and/or position */
  C_LPP_CP_CF_STATE_TIMER_MEAS_RESP_TIME  = 0x0002,

  /* Waiting for OTDOA to produce measurements position */
  C_LPP_CP_CF_STATE_TIMER_OTDOA_RESP_TIME = 0x0004,

  /* Waiting for RRC to provide cell info */
  C_LPP_CP_CF_STATE_CELL_INFO_REQ         = 0x0008,

  /* Waiting for OTDOA meas when OTDOA is polled */
  C_LPP_CP_CF_STATE_OTDOA_MEAS_REQ        = 0x0010,
  /* Waiting for ECID meas when ECID is polled */
  C_LPP_CP_CF_STATE_ECID_MEAS_REQ         = 0x0020,
  /* Waiting for AD to be sent by the SMLC */
  C_LPP_CP_CF_STATE_TIMER_AD_REQ          = 0x0040,
  /* First OTDOA RSTD measurement received, but more is expected for completion */
  C_LPP_CP_CF_STATE_OTDOA_MUL_MEAS_RECV   = 0x0080,
} tm_lpp_cp_cf_state_enum_type;


typedef enum
{
  C_LPP_CP_SESSION_NONE,
  C_LPP_CP_SESSION_NI, /* network-initiated */
  C_LPP_CP_SESSION_UI, /* UE-initiated immediate */
} tm_lpp_cp_callflow_kind_enum_type;

typedef struct
{
   boolean   u_final;
   uint32    q_fix_time_gps_msec; /* gps time: mses in week */
   uint32    q_fix_time_utc_msec; /* time adjusted for leap seconds */
   uint32    q_glonass_tod_modulo_1hour_ms_modulo_1hour_ms; /* GLONASS TimeOfDay:
                               its valid range GANSSTODm ::= INTEGER (0 .. 3599999);
                             */

   uint16    w_GpsWeek;             /* fix week number in GPS time */

   uint32    q_pos_sources; /* indicate GPS/GLONASS etc. */

   gad_ellip_alt_unc_ellip_type   z_pos;
   /* used for RRLP and LPP. 
      For LPP the lat/long/alt are not used from this structure.
      all remaining data is used for LPP also
   */
   boolean   vel_valid;
   float     f_VelEnu[3];         /* East, North, Up velocity (m/s). */
   float     f_Vunc3dMps;
   float     f_VelVertUnc;
   float     f_ClockBias;         /* Receiver's clock bias (m).    */
   float     f_ClockBiasUncMs;    /* Receiver's clock uncertainty (m). */
   uint16    w_Alt;               /* GAD shape values stored for LPP. 
                                     all remaining values are used from 
                                     gad_ellip_alt_unc_ellip_type */
   uint32    q_Lat;               /* GAD shape values stored for LPP. 
                                     all remaining values are used from 
                                     gad_ellip_alt_unc_ellip_type */
   int32     l_Long;              /* GAD shap values stored for LPP. 
                                     all remaining values are used from 
                                     dad_ellip_alt_unc_ellip_type */
   // Glonass
   uint32 q_GloTimeMs;     /* Glonass time in day (milliseconds). 0..3599999. Valid only when q_pos_sources is Glonass only (C_POS_SOURCE_GLONASS) */
} tm_lpp_cp_pending_pos_to_network_s_type;

typedef struct
{
   boolean                                  u_valid;
   tm_lpp_cp_pending_pos_to_network_s_type  z_pos_info;
}tm_lpp_cp_cached_pos_for_net_s_type;


/* for q_lpp_flags in tm_lpp_cp_data_type */
/* LPP-CP has received from the Engine valid */
#define TM_LPP_CP_GPS_PRM_VALID                         0x00000001 /*     GPS PRM */
#define TM_LPP_CP_GLO_PRM_VALID                         0x00000002 /*     Glonass PRM */
#define TM_LPP_CP_ECID_VALID                            0x00000008 /*     ECID measurements */

#define TM_LPP_CP_AD_REQUESTED                          0x00000010 /* LPP CP has requested Assitance Data from the engine */
#define TM_LPP_CP_AD_RECVD                              0x00000020 /* LPP CP has received Assistance Data from the engine */

#define TM_LPP_CP_OTDOA_DELIVERED                       0x00000040 /* OTDOA assistance data delivered */
#define TM_LPP_CP_MAY_RET_ADD_INFO                      0x00000080
#define TM_LPP_CP_MAY_REQ_ADD_AD                        0x00000100 /* UE may request additional OTDOA AD from the server */
#define TM_LPP_CP_REQ_LOC_INFO_RCVD                     0x00000200 /* LPP CP has received IE Req Loc Info from E-SMLC */
/* Location Information IE from SMLC contains req for */
#define TM_LPP_CP_REQ_LOC_INFO_POS_EST_RCVD             0x00000400 /*     Position Estimate from UE (i.e. MSB) */
#define TM_LPP_CP_REQ_LOC_INFO_GNSS_MEAS_RCVD           0x00000800 /*     Measurements, i.e. position computed by E-SMLC, MSA */
#define TM_LPP_CP_REQ_LOC_INFO_OTDOA_RCVD               0x00001000 /*     OTDOA measurements */
#define TM_LPP_CP_REQ_LOC_INFO_ECID_RCVD                0x00002000 /*     ECID measurements */

/* Combine the following information elements when sending
 * out to E-SMLC */
#define TM_LPP_CP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA_ECID 0x00004000 /*     AGNSS+OTDOA+ECID measurements */
#define TM_LPP_CP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA      0x00008000 /*     AGNSS+OTDOA measurements */
#define TM_LPP_CP_REQ_LOC_INFO_COMBINE_AGNSS_ECID       0x00010000 /*     AGNSS+ECID measurements */
#define TM_LPP_CP_REQ_LOC_INFO_COMBINE_OTDOA_ECID       0x00020000 /*     OTDOA+ECID measurements */

#define TM_LPP_CP_REQ_LOC_INFO_VEL_REQUESTED            0x00040000 /*     Velocity requested */

/*     An A-GNSS callflow, UE-B or UE-A. */
#define TM_LPP_CP_REQ_LOC_INFO_AGNSSS_RCVD              (TM_LPP_CP_REQ_LOC_INFO_GNSS_MEAS_RCVD|TM_LPP_CP_REQ_LOC_INFO_POS_EST_RCVD)


typedef struct
{
   /* See TM_LPP_CP_xxx above */
   uint32                    q_lpp_flags;
   /* list OTDOA data struct contains one or more RSTD measurements.
    * Maximum of TM_LPP_MAX_OTDOA_RSTD_MEAS_DATA RSTD measurements is supported.
    */
   tm_otdoa_data_store      z_otdoa_data;

   uint8                     u_lpp_error_type;
   uint8                     u_lpp_error_code;

   /* this value is set to TRUE if UE can request additional OTDOA AD */
   boolean                   b_ok2req_add_otdoa_ad;

   /* this value is set to TRUE if UE can request additional GNSS AD */
   boolean                   b_ok2req_add_gnss_ad;

   boolean                   b_status_check_needed;
   uint32                    q_lpp_ecid_req_loc;
   gnss_lpp_ECID_ProvideLocationInformation   z_lpp_ecid_data;
   SmGnssMeasReportStructType                 z_gnss_meas_report;

   /* Hold Auxiliary info AD until AcqAssist is available to process & post the
    * steering info to MGP */
   GNSS_AuxiliaryInformation  *p_gnss_auxi_info_cache;
   /* Store AcqAssist AD until GPS UTCModel or Glonass TimeModel AD is available */
   GNSS_AcquisitionAssistance  z_gnss_acqassist_cache;
} tm_lpp_cp_data_type; 

/*
 *  This structure contains all "per-session" LPP CP data with TM-Core.  
 *  It is cleared at the end of each session.
 */
typedef struct
{
   /*  +
    *  PDAPI ID 
    */
   pdsm_client_id_type                           q_pdapi_client_id;

   /*  + (tm_common.h)
    *  TM session ID/Handle
    */
   tm_sess_handle_type                           tm_active_session_handle;

   /*  +
    *  This is a copy of the Start Session Request parameters that
    *  were used to start the session, assuming it is an MO session.
    *  This field will be left blank if this is an MT session.
    */
   prtl_start_sess_req_param_s_type              z_start_sess_req_param;

   /*  +
    *  Copy of the loc_info_req_type sent by the SMLC, one of 
    *  LPP_locationMeasurementsRequired, MeasurementsPreferred, and 
    *  EstimateRequired, EstimatePreferred.
    */
   LPP_LocationInfoType                          u_orig_loc_info_req_type;

   /* #############  OK  #################
    *  Session QoS information
    */
   boolean                                       b_qos_valid;
   tm_lpp_cp_session_qos_type                    z_session_qos;

   /*  (tm_umts_common_utils.h)
    *  GPS Assistance Data is encoded into Gsm0931 formatting and
    *  stored in here for use later.
    */
   GpsAssisData_Gsm0931                          z_GpsAssistData;

   GpsAssisData_ganss_Gsm0931                    z_GloAssistData; /* Glonass assist. data wishlist */

   /* buffer to combine Glonass eph. data with the freq. number in aux. info assistance */
   prot_glo_eph_buffer_s_type                    z_prot_glo_eph_buffer[N_GLO_SV];

   /* buffer to combine Glonass Acq. Assistance data with the freq. number in aux. info assistance */
   prot_glo_acqast_buffer_s_type                 z_prot_glo_acqasst_buffer;

   /* Cached (pending) position
      Used when a position is reported to pdapi prior to completion of the 
      network call flow 
    */
   tm_lpp_cp_cached_pos_for_net_s_type           z_pending_pos_to_network;

   /* ayt_todo  - to remove
    *  Cached (pending) position stored when PDAPI position is returned
    *  to the GSM-CP protocol sub-module before an MPR is received.
   umts_cp_gsm_pending_pos_type                  z_pending_pos;
    */

   /*  (tm_umts_common_utils.h)
    *  Assistance Data is stored here
    */
   tm_network_assist_s_type                      z_network_assist;

   /*  ayt_todo
    *  Used for time-stamping MS-Assisted position
   msa_pos_time_stamp_type                       z_msa_pos_time_stamp;
    */

   gnss_time_info                             z_set_assist_pos_time_stamp;  // ayt_todo - can this combine with above z_msa_pos_time_stamp ?
                                                                            // ayt_todo - this captures the REF TIME from Prov AD

   /*  +
    *  NI vs. UI 
    */
   tm_lpp_cp_callflow_kind_enum_type           e_callflow_kind;

   /*  +
    *  UE-BASED vs UE-ASSISTED
    */
   tm_lpp_cp_session_agps_mode_enum_type         e_agps_mode;

   /*  +
    *  Bit map containing a bit mask of GNSS methods requested by the SMLC
    */
   uint8                                         u_needed_gnss_method_bmap;
   /*
    *  This defines the various callflow states for the GSM-CP protocol
    *  sub-module.
    */
   tm_lpp_cp_cf_state_enum_type                 e_cf_state;

   /* ayt_todo
    *  Either LOCATION_ESTIMATE or ASSISTANCE_DATA.  This is used to 
    *  define what type of MO callflow is used.
   molr_type_e_type                              e_mo_lr_type;
    */


   /*
    *  This is a bit mask describing the assistance data pieces that
    *  have been received this session.  It is used in MTMA mode to 
    *  determine what wishlist should be requested from the server.
    *  The bit mask is defined by the #defines in sm_api.h which all
    *  start with C_GPS_ASSIS_.  
    */
   uint32                                        q_agps_data_rcvd;
   uint32                                        q_aglo_data_rcvd;

   /*
    * flag to keep track of leap second provision by SMLC. If this is not
    * provided, then it shall be requested from the SMLC.
    */
   //boolean                                       b_leap_second_rcvd;

   tm_lpp_cp_data_type                           z_lpp_cp_data;

   /*
     *  This is a bit mask describing which almanac and ephemeris data
     *  has been received.  It is used to provide a list of non-existent
     *  or unhealthy SVs to MGP.
     */
   uint32                                        q_alm_sv_list;
   uint32                                        q_eph_sv_list;

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

   /* Cache the original wish list */
   sm_InternalAssistDataStatusStructType         z_OrigWishList;     // ayt_todo - check if logic is applicable

   /*
    *  Set to TRUE when a session is started with TM-Core.
    */
   boolean                                       u_tm_core_session_started;

   /*  ayt_todo
    *  Set to TRUE when ref location is to be acquired BY FORCE
   boolean                                       u_force_ref_loc;
    */

   /*  +
    *  Set to TRUE when a network session is in progress
    */
   boolean                                       u_network_session_on;

   /* LM has been given the true qos_timer */
   boolean                                       qos_timer_started;

   /*
    *  Set to TRUE when accuracy reset timer expires
    */
   boolean                                       u_accuracy_reset_timer_expired;


   /* ayt_todo
    *  The u_preemption_flag is set when a GSM preemption event is
    *  received.  It is never cleared until the end of the session.  This
    *  flag can be used to alter logic which is dependent on whether or
    *  not there has been at least one preemption in the current session.
   boolean                                       u_preemption_flag;
    */

   /*----------------------------------------------*/
   /* The following is used for Periodic Reporting */
   /*----------------------------------------------*/
   /* from Req Loc Info Msg: when Common IE for periodic reporting is present */
   boolean                                       u_periodic_multi_report;

   /* from Req Loc Info Msg: total number of reports in a multiple-reporting periodic session */
   uint16                                        w_cp_periodic_reports_total_num;

   /* Number of remaining reports in a multi-reporting periodic session.
      Initialized to w_cp_periodic_reports_total_num.
    */
   uint16                                        w_cp_periodic_reports_remaining;

   /* Count how many fix or no-fix reports before the 1st or since last reporting */
   uint16                                        w_lm_report_count;

   /* */
   uint32                                        q_cp_periodic_reporting_interval_sec;

   /* Meas Repsonse time in sec. */
   uint16                                        w_cp_reporting_interval_long;

   /* pd comm begin sent to PDAPI*/
   boolean                                       b_comm_begin_sent;
   /* pd comm begin sent to PDAPI*/
   boolean                                       b_comm_end_sent;

   /* 
    * Store the transaction for a AD req sent to SMLC. This will be used for
    * substitute ACK and also to time out pending transactions
    */
   tm_lpp_transaction_info_s_type *p_req_ad_tx;
   /* boolean value tracking if OTDOA AD has been requested in this session */
   boolean                                       b_otdoa_ad_requested;
   /* bit mask tracking if GNSS AD has been requested in this session */
   uint32                                        q_gps_ad_requested;
   /* bit mask tracking if GNSS AD has been requested in this session */
   uint32                                        q_glo_ad_requested;

   /* if AD wishlist request is pending from SM to MGP */
   boolean                                       b_ad_wishlist_req_pending;
   /* if AD request is pending to be sent to server */
   boolean                                       b_req_ad_pending;
} tm_lpp_cp_session_status_s_type;


/* LPP Control Plane Protocol Info */
typedef struct {
  tm_lpp_error_type                     lpp_error;   /* Used for LPP Error Message */
  uint8    u_transaction_id;            /*  Tx Id being processed                */
  boolean  u_is_duplicate;              /*  TRUE - Found a duplicate LPP message */
  boolean  u_ack_requested;             /*  TRUE - ACK requested                 */
  boolean  u_last_rcvd_seq_num_valid;   /*  Is the Last Received Sequence Number valid ?  */  
  uint8    u_last_rcvd_seq_num;         /*  Last Received Sequence Number : 0 .. 255      */
  uint8    u_last_out_seq_num;          /*  Last Outgoing Sequence Number : 0..255        */
  boolean  u_last_out_seq_num_valid;    /*  Is the Last Outgoing Sequence Number valid ?  */
  
  boolean b_wait_for_ack;
} tm_lpp_cp_info_s_type;

extern tm_lpp_provide_cap_s_type   z_lpp_cp_provide_cap;  /* tm_lpp_provide_cap_s_type in tm_lpp_tx_mgr.h */
extern boolean                     b_ota_reset_flag;      /* TRUE if an OTA reset is rcvd, indicating test mode */

/*===========================================================================
FUNCTION tm_lpp_cp_check_duplicate

DESCRIPTION
  This function verifies the sequence number is not a duplicate of the 
  last rcvd sequence number.

PARAMETERS:   seq_number      -  sequence number to check if this is a duplicate
              transaction_id  -  transaction id of message for duplicate check

DEPENDENCIES: none.

RETURN VALUE: TRUE - duplicate;  FALSE - not a duplicate

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_lpp_cp_check_duplicate( uint8 seq_number, uint8 transaction_id );

/*===========================================================================

FUNCTION tm_lpp_cp_init

DESCRIPTION
  This function initializes LPP Control Plane Protocol Module.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_init( void );


/*===========================================================================

FUNCTION tm_lpp_cp_error_cb

DESCRIPTION
  This function is the LPP Control Plane Protocol error callback to handle
  LPP Reliable Transport errors for transmition failures, etc.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_error_cb(
                         int32  error,      /* error number defined above */
                         uint32 nas_id,     /* NAS Routing ID for associated outgoing LPP packet */
                         uint32 trans_id,   /* Transaction ID for associated outgoing LPP packet */
                         uint8  seq_num     /* Sequence Number for associated outgoing LPP packet */
                        );


/********************************************************************
*
* tm_lpp_cp_lte_cell_db_update
*
* Function description:
*   Processing cell db update from MGP for LTE or OOS.
*
* Parameters: 
*
* Return:
*  None
*  
*********************************************************************
*/
void tm_lpp_cp_lte_cell_db_update( const cgps_CellInfoCachedStruct *cell_db_data_ptr );


#ifdef FEATURE_CGPS_LTE_CELLDB
/*===========================================================================

FUNCTION
  tm_lpp_cp_lte_cell_meas_update

DESCRIPTION
  This function copies the cell-measurements sent from the LTE L1, into the
  database (if they are valid).

PARAMETERS

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/
void tm_lpp_cp_lte_cell_meas_update( lte_ml1_gps_mrl_cell_meas_update_t *cell_meas_update_ptr );
#endif /* FEATURE_CGPS_LTE_CELLDB */

/********************************************************************
 *
 * tm_lpp_cp_pa_proc
 *
 * Function description:
 *  Processes PA events from TM Core.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */
void tm_lpp_cp_pa_proc( prtl_pa_event_s_type *p_pa_event  );


/*===========================================================================
FUNCTION tm_lpp_cp_ack_requested

DESCRIPTION
  This function updates the flag in TM LPP CP INFO on ACK Requested flag.

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_ack_requested( boolean u_ack_requested );


/*===========================================================================
FUNCTION tm_lpp_cp_wait_for_ack

DESCRIPTION
  This function updates the flag in TM LPP CP INFO on ACK is received

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_wait_for_ack( boolean u_ack_rcvd );

/********************************************************************
 *
 * tm_lpp_cp_get_nxt_out_seq_no 
 *
 * Function description: 
 *   This function gets the next outgoing sequence number for a LPP Message.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */
void tm_lpp_cp_get_nxt_out_seq_no( tm_lpp_transaction_info_s_type *p_lpp_tx_info_type );

/*===========================================================================

FUNCTION tm_lpp_cp_handle_nofix

DESCRIPTION handles TmCore provided no-fix info: only used for multiple 
  periodic reporting.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_lpp_cp_handle_nofix( void );

/*===========================================================================

FUNCTION tm_lpp_cp_handle_lr_data

DESCRIPTION handles TmCore provided LR data.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_lpp_cp_handle_lr_data( sm_GnssFixRptStructType *lr_data_ptr, boolean u_final );


/*===========================================================================

FUNCTION tm_lpp_cp_handle_prm_data

DESCRIPTION handles TmCore provided PRM data.

DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_lpp_cp_handle_prm_data( sm_GpsMeasRptStructType *p_prm_data );

/********************************************************************
 *
 * tm_lpp_cp_handle_otdoa_timer_expiry
 *
 * Function description: 
 *   This is LPP Control Plane Event handler which handles timer expiry
 *   for OTDOA timer.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */

void tm_lpp_cp_handle_otdoa_timer_expiry(
                                          tm_lpp_cp_cf_state_enum_type current_state
                                         );

/*===========================================================================
FUNCTION tm_lpp_cp_handle_otdoa_mul_meas_timer_expiry

DESCRIPTION
  This function handles Multi-OTDOA Timer expiry. Remove the transaction for the
  AD req from tx manager. If no more pending transactions, start a 
  NAS_SESS_TIMER for 5 seconds.

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_lpp_cp_handle_otdoa_mul_meas_timer_expiry( tm_lpp_cp_cf_state_enum_type const eCurrentState );

/*===========================================================================
FUNCTION tm_lpp_cp_handle_ad_timer_expiry

DESCRIPTION
  This function handles AD Timer expiry. Remove the transaction for the
  AD req from tx manager. If no more pending transactions, start a 
  NAS_SESS_TIMER for 5 seconds.

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_lpp_cp_handle_ad_timer_expiry( void );

/********************************************************************
 *
 * tm_lpp_cp_handle_ecid_timer_expiry
 *
 * Function description: 
 *   This is LPP Control Plane Event handler which handles timer expiry
 *   for ECID timer.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */

void tm_lpp_cp_handle_ecid_timer_expiry( void );
/********************************************************************
 *
 * tm_lpp_cp_handle_nas_sess_timer_expiry
 *
 * Function description: 
 *   This is LPP Control Plane Event handler which handles timer expiry
 *   for NAS session timer.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */

void tm_lpp_cp_handle_nas_sess_timer_expiry( void );
/********************************************************************
 *
 * tm_lpp_cp_events
 *
 * Function description: 
 *   This is LPP Control Plane Event handler which serves as the 
 *   entry point of processing incoming LPP Control Plane Messages.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */
void tm_lpp_cp_events( prtl_lpp_cp_event_s_type *lpp_cp_event_info );

/*
******************************************************************************
* tm_lpp_cp_handle_otdoa_data
*
* Function description:
*
*   Handle OTDOA data 
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void tm_lpp_cp_handle_otdoa_data( sm_loc_lte_otdoa_meas_info_type const *p_OtdoaData );


/*
******************************************************************************
* tm_lpp_cp_handle_ecid_data
*
* Function description:
*
*   Handle OTDOA data 
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void tm_lpp_cp_handle_ecid_data( gnss_lpp_ECID_ProvideLocationInformation *p_otdoa_data );
/*===========================================================================

FUNCTION tm_lpp_cp_get_nas_id

DESCRIPTION
   This function returns the current NAS session id if available.

DEPENDENCIES
   None.

RETURN VALUE
   TRUE - a valid NAS id is available; FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_lpp_cp_get_nas_id( uint32 *p_nas_id );

/*===========================================================================

FUNCTION tm_lpp_cp_get_as_id

DESCRIPTION
   This function returns the subscription id if available.

DEPENDENCIES
   None.

RETURN VALUE
   TRUE - a valid AS id is available; FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_lpp_cp_get_as_id( sys_modem_as_id_e_type *p_as_id );

/*===========================================================================
FUNCTION tm_lpp_cp_process_ack

DESCRIPTION
  This function process Server Ack (implicit or explicit).

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_lpp_cp_process_ack( LPP_Message                     *p_pdu_decoded_ptr,
                               tm_lpp_transaction_info_s_type **p_lpp_tx_type,
                               tm_lpp_error_type               *p_lpp_error,
                               boolean                         *p_early_exit );
/*
******************************************************************************
* tm_lpp_cp_send_prov_loc
*
* Function description:
*
*   Sends a provide location information message to server
*
* Parameters: flags to indciate the type of message.
*   
* Return value: None.
*
******************************************************************************
*/

void tm_lpp_cp_send_prov_loc( uint32 flags);

/*
******************************************************************************
* tm_lpp_cp_send_lpp_error
*
* Function description:
*
*   Sends an error message to server
*
* Parameters: 
*   
* Return value: None.
*
******************************************************************************
*/
void tm_lpp_cp_send_lpp_error(void);

/*===========================================================================
FUNCTION tm_lpp_cp_set_cf_state

DESCRIPTION
  This function sets CP call flow state.

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_set_cf_state( tm_lpp_cp_cf_state_enum_type e_new_cf_state,
                             uint32                       q_time_out );  // ayt_todo - check update

/*===========================================================================
FUNCTION tm_lpp_cp_terminate_timer_cancel_cf_state

DESCRIPTION
  This function terminates CP call flow state.

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_terminate_timer_cancel_cf_state( tm_lpp_cp_cf_state_enum_type e_timer_state_to_terminate );

/*===========================================================================
FUNCTION tm_lpp_cp_handle_cf_timer_expiry

DESCRIPTION
  This function handles CF Timer expiry.

PARAMETERS:   

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_lpp_cp_handle_cf_timer_expiry( void );

/*===========================================================================

FUNCTION tm_lpp_cp_inform_tm_core_stop_session

DESCRIPTION
  This function informs the TM core that the current LPP CP session should 
  terminate.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_inform_tm_core_stop_session( tm_sess_stop_reason_e_type stop_reason );


/*
 ******************************************************************************
 * tm_lpp_cp_send_lpp_abort
 *
 * Function description:
 *  Sends LPP ABORT for all outstanding transactions
 *
 * Parameters:
 *
 * Return value:
 *      TRUE - if successfully sent, FALSE otherwise
 ******************************************************************************
*/
boolean tm_lpp_cp_send_lpp_abort( void );


/********************************************************************
 *
 * tm_lpp_cp_emergency_exit
 *
 * Function description: 
 *   This function handles LPP Control Plane periodic session after an emergency
 *   call ends  
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */
void tm_lpp_cp_emergency_exit( void );

/********************************************************************
 *
 * tm_lpp_cp_send_req_ad
 *
 * Function description: 
 *   This function sends a request for AD.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */
void tm_lpp_cp_send_req_ad( void );

/********************************************************************
 *
 * tm_lpp_cp_config_change_proc
 *
 * Function description:
 * This function processes change to the LPP COnfig value via QMI/NV
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */
void tm_lpp_cp_config_change_proc(pdsm_pa_lpp_config_info_type lpp_config_info);


#endif
