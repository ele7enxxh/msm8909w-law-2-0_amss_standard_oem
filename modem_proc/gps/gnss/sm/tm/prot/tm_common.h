#ifndef TM_COMMON_H
#define TM_COMMON_H

#include "gps_variation.h"

/*===========================================================================

                  TM_COMMON Header File

DESCRIPTION
  This header file contains definitions of common data structure necessary for
  TM module.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/01/15   rk      Define all position protocols.
08/27/15   pa      Removing unsued Leap Second constants 
06/18/15   jv      Added tm_util_clean_qzss_sbas_meas_rpt()
05/21/15   nl      Refactor code
01/07/15   jv      QZSS Support
12/04/12   rk      CR 408753 - Update tm_lr_resp_type struct to include SV extn params.
07/31/12   mjohn   Removed NMEA_GPSLEAP_SECOND and GPS_UTC_LEAP_SECS
02/16/12   gk      Altitude also reported in float now
11/23/11   ssu     Added GFC_QMI feature.  
06/16/11   rh      Added TM_PHONE_EVENT_OTASP_STATUS_COMMITTED event
01/03/11   gk      Eph throttling rework
11/03/10   rh      Added horizontal/vertical velocity uncertainty calc routines
03/03/10   ns      DSDS Support
12/2/09    gk      intermediate position fixes
09/01/09   gk      Added a check to validate the directory for EFS operation
12/24/08    ns     Added timer for QWip System determination  
11/06/08   atien   NMEA_GPSLEAP_SECOND, GPS_UTC_LEAP_SECS  update to 15 seconds
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/25/08    gk     On-demand changes
02/06/08    cl     Added accommodating "forced TM termination".
08/01/07    rw     Add support for XTRA feature
08/02/07    LT     Added a utility function for handling SVs non-existence case.
07/30/07    ank    Added support for ext_pos_data in Position Report.
07/17/07    lt     L1 interface.
06/27/07    lt     Added the passing of WCDMA in-service to UMTS submodules.
06/15/07    ank    Added TM_MAX_SESSION_TIMEOUT.
05/25/07    lt     Added SMG_IDs for MM and RRC; Added two utility functions.
05/09/07    ank    Changed prototype of tm_util_pd_info_data_init
02/01/07    lt     Corrected a typo in the constant of TM_UMTS_UP_TRANSACTION_ID_RANGE
12/22/06    ank    Added support for MSBased Throttling feature.
07/12/06    cl     Initial Release
===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#include "aries_os_globals.h"
#include "aries_os_api.h"
#ifdef FEATURE_CGPS_UMTS_CP
  #include "cm.h"
  #ifdef FEATURE_CMI
    #include "cm_v.h"
  #else
    #include "cmwmsg.h"
  #endif
#endif
#include "mgp_api.h"
#include "sm_api.h"
#include "pdapi.h"
#include "gps_consts.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SM_MEAS_STATUS_SM_VALID      (MEAS_STATUS_SM_VALID)        /* TRUE -> f_SatSubMs is valid */
#define SM_MEAS_STATUS_FROM_RNG_DIFF (MEAS_STATUS_FROM_RNG_DIFF)   /* TRUE -> Last update from range diff, not range meas */
#define SM_MEAS_STATUS_FROM_VE_DIFF  (MEAS_STATUS_FROM_VE_DIFF)    /* TRUE -> Last update from velocity diff, not vel meas */
#define SM_MEAS_STATUS_XCORR         (MEAS_STATUS_XCORR)           /* TRUE -> Strong indication of cross correlation */
#define SM_MEAS_STATUS_DONT_USE      (MEAS_STATUS_DONT_USE) /* TRUE -> Dubious measurement, wait for another dwell */
#define SM_MEAS_STATUS_INVALID_FLAGS (SM_MEAS_STATUS_XCORR | \
                                      SM_MEAS_STATUS_DONT_USE | \
                                      SM_MEAS_STATUS_FROM_RNG_DIFF | \
                                      SM_MEAS_STATUS_FROM_VE_DIFF )

/* Definitions used to obtain gps weeks and milliseconds within the week. */
#define MINUTES_PER_WEEK    ( 7 * 24 * 60 )
#define MS_PER_MINUTE       ( 60 * 1000 )
#define MS_PER_WEEK         ( MINUTES_PER_WEEK * MS_PER_MINUTE )
#define SEC_PER_WEEK        (60*60*24*7)

#define TENTH_MSECS         ((uint32)100)
#define SEC_MSECS           ((uint32)1000)
#define MIN_MSECS           ((uint32)60*SEC_MSECS)
#define HOUR_MSECS          ((uint32)60*MIN_MSECS)

/* Define hex negative one in 32bits*/
#define HEX_NEGATIVE_ONE 0xFFFFFFFF


/* Gpsone time stamp related definitions.
 *
 * To obtain the number below, the range specified in time_get (time_svc.c),
 * 0 - 768 * 64, spanning 1.25 ms, is first rescaled to the range
 * 0 - 960 * 64, and one fifth of that value is used.
*/
#define QUARTER_MS_VAL      ( 192 * 64 )

/* Definitions used in formatting navigation fix. */
#define MAX_POS_UNC_IND 32
#define HEADING_SCALE (128.0 / 45.0)

#define IS801_CLOCK_BIAS_OFFSET     13000   /* In nsec */
#define LOC_UNC_CONFIDENCE          39       /* PE uses a standard error ellipse and the confidence level 
                                              for a standard error ellipse is defined as 39% */
#define ME_LOC_UNC_CONFIDENCE       63       /* ME uses a standard 2D Circular and the confidence level 
                                              for a standard error ellipse is defined as 39% */
#define C_2_32_OVER_PI              (4294967296.0 / C_PI )
#define C_2_31_OVER_PI              (2147483648.0 / C_PI )

#define GLO_FREQ_NUMBER_OFFSET	7

/*  The SV ID of a GPS SV is within the range defined for GPS SVs in MGP:     1 - 32. */
#define PRN_OK(x)           ( ((x)>0) && ((x)<(N_GPS_SV+1)) )
/*  The SV ID of a Glonass SV is within the range defined for Glonass SVs in     MGP: 65 - 88. */
#define SLOTID_OK(x)        ( ((x)>GLO_SLOTID_OFFSET) && ((x)<(N_GLO_SV+GLO_SLOTID_OFFSET+1)) )
/*  The frequency number of a Glonass SV is not within the range defined for     Glonass SVs in MGP: 0 - 13. */
#define GLO_SIGNED_FREQ_INVALID(x) ((x)<GLO_LOWEST_FREQ_NUMBER || (x)>GLO_HIGHEST_FREQ_NUMBER )
/*  The SV ID of a BDS SV is  within the range in MGP: 201 - 237. */
#define BDS_ID_OK(x)        ( ((x)>BDS_SV_ID_OFFSET) && ((x)<=(N_BDS_SV+BDS_SV_ID_OFFSET)) )
/*  The SV ID of a GAL SV is  within the range in MGP: 301 - 336. */
#define GAL_ID_OK(x)        ( ((x)>GAL_SV_ID_OFFSET) && ((x)<=(N_GAL_SV+GAL_SV_ID_OFFSET)) )


#define NV_GPS_MEAS_REPORT_ENABLED     0x01
#define NV_GLO_MEAS_REPORT_ENABLED     0x02
#define NV_BDS_MEAS_REPORT_ENABLED     0x04
#define NV_GAL_MEAS_REPORT_ENABLED     0x08
#define NV_QZSS_MEAS_REPORT_ENABLED    0x10

/**
 * @brief Calendar time with tenth of second accuracy.
 *
 * This is a legacy structure that can represent time with tenth of second
 * accuracy in a form of Gregorian calendar fields.
 *
 * @deprecated Use tm_TimeT instead where applicable.
 * @sa tm_TimeT
 */
typedef struct
{
  uint32 q_year;
  uint32 q_month;
  uint32 q_utc_days;
  uint32 q_hours;
  uint32 q_mins;
  uint32 q_secs;
  uint32 q_tenths;
}utc_time;

/**
 * @brief Calendar time with milliseconds
 *
 * Gregorian calendar time structure. This structure can be converted into and
 * from GPS, GLO and UTC timestamps.
 *
 * @sa utc_time
 */
typedef struct
{
  uint32 q_Year;       /**< @brief Year 1970+ */
  uint32 q_Month;      /**< @brief Month 1-12 */
  uint32 q_Day;        /**< @brief Day in month 1-31 */
  uint32 q_Hour;       /**< @brief Hour   0-23 */
  uint32 q_Minute;     /**< @brief Minute 0-59 */
  uint32 q_Second;     /**< @brief Second 0-59 */
  uint32 q_Millisecond;/**< @brief Millisecond 0-999 */
} tm_CalendarTimeT;

/* Store GPS, Glonass time provided by network */
typedef struct
{
  struct
  {
    unsigned refTimeWeekNumValid : 1;   /* Set 1 for w_ref_time_week_num to be valid */
    unsigned refTimeTowMsValid : 1;     /* Set 1 for q_ref_time_tow_ms to be valid */
    unsigned posTimeStampValid : 1;     /* Set 1 for t_pos_time_stamp to be valid */
    unsigned deltaTPresent : 1;         /* Set 1 for b_deltaT to be valid */
    unsigned glonassTimeValid : 1;      /* Set 1 for z_GlonassTime to be valid */
  } m;

  uint16 w_ref_time_week_num;         /* GPS week number. Valid when refTimeWeekNumValid == 1  */
  uint32 q_ref_time_tow_ms;           /* GPS of week in milli second (0..604799999). Valid when refTimeTowMsValid == 1 */
  uint64 t_pos_time_stamp_ms;         /* Milli seconds since birth of GPS time. Valid when pos_time_stamp_valid = 1 */
  int8 b_deltaT;                      /* Leap second offset time from network (-128..127). Used to get GPS or GLONASS
                                         from another GNSS system. Valid when deltaTPresent == 1 */
  glo_TimeStructType z_GlonassTime;   /* GLONASS time structure. Valid when glonassTimeValid == 1 */
} gnss_time_info;

extern const FLT f_PosUncArr[];

typedef struct
{
  uint16 w_GpsWeek;            /* GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;            /* GPS msec at reference tick [msecs] */
  float f_ClkTimeBias;        /* Clock bias [msecs] */
  float f_ClkTimeUncMs;       /* 1 sided max time bias uncertainty [msecs] */
  uint16 w_MsAmbig;            /* msec ambiguity in time [msec] */
} me_Time;

typedef struct
{
  float f_ClkFreqBias;  /* Clock frequency bias. [m/s] */
  float f_ClkFreqUnc;   /* 1 sided max freq bias uncertainty [m/s] */
} me_Freq;

typedef struct
{
  uint32 q_RefFCount;        /* FCount value at reference tick */
  me_Time z_Time;             /* Time at w_RefFCount */
  me_Freq z_Freq;             /* Freq at w_RefFCount */
} me_Clock;

/*--------------------------------------------------

   Common Defines for TM Modules

---------------------------------------------------*/
#define TM_MAX_BUFFSIZE                     200 /* Max PDDM Size */
#define TM_MAX_SESSION_TIMEOUT              255 /* Max Session Timeout */
#define TM_MAX_EMERGENCY_SESSION_TIMEOUT    40 /* Max Session Timeout for Emergency session */

/* Standard certificate pathname for SUPL*/
#define TM_UMTS_SUPL_ROOT_CERT_DIR          "/SUPL/"
#define TM_UMTS_SUPL_ROOT_CERT_DIR_PATH_LEN (sizeof(TM_UMTS_SUPL_ROOT_CERT_DIR) -1)


/*--------------------------------------------------

    Timer ID range allocation for each modules

---------------------------------------------------*/
/* Each TM software module is assigned a timer ID range */
#define TM_CORE_TIMER_ID_RANGE                0x00000000
#define TM_STANDALONE_TIMER_ID_RANGE          0x01000000
#define TM_1X_UP_TIMER_ID_RANGE               0x02000000
#define TM_1X_CP_TIMER_ID_RANGE               0x03000000
#define TM_UMTS_UP_TIMER_ID_RANGE             0x04000000
#define TM_UMTS_CP_TIMER_ID_RANGE             0x05000000
#define TM_PDAPI_TIMER_ID_RANGE               0x06000000
#define TM_DIAG_TIMER_ID_RANGE                0x07000000
#define TM_CM_IFACE_TIMER_ID_RANGE            0x08000000
#define TM_SEC_LAYER_TIMER_ID_RANGE           0x09000000
#define TM_XTRA_TIMER_ID_RANGE                0x0A000000
#define TM_SYSD_TIMER_ID_RANGE                0x0B000000
#define TM_SENSOR_ID_RANGE                    0x0C000000
#define TM_XSPI_ID_RANGE                      0x0D000000
#define TM_PERIODIC_PPM_TIMER_ID_RANGE        0x0E000000
#define TM_LPP_CP_TIMER_ID_RANGE              0x0F000000

/*--------------------------------------------------

      Supported Protocol Types by TM-Core            

---------------------------------------------------*/
#define TM_PRTL_NUM                 11
typedef uint32 tm_prtl_type;
#define TM_PRTL_TYPE_NONE           0x0000
#define TM_PRTL_TYPE_STANDALONE     0x0001
#define TM_PRTL_TYPE_1X_UP          0x0002
#define TM_PRTL_TYPE_1X_CP          0x0004
#define TM_PRTL_TYPE_UMTS_UP        0x0008
#define TM_PRTL_TYPE_UMTS_CP        0x0010
#define TM_PRTL_TYPE_DIAG           0x0020
#define TM_PRTL_TYPE_PPM            0x0040
#define TM_PRTL_TYPE_GFC_QMI        0x0080
#define TM_PRTL_TYPE_XTRA_T         0x0100
#define TM_PRTL_TYPE_LPP_CP         0x0200
#define TM_PRTL_TYPE_E911           0x0400


#define TM_PRTL_TYPE_ALL            (TM_PRTL_TYPE_STANDALONE|\
                                     TM_PRTL_TYPE_1X_UP|\
                                     TM_PRTL_TYPE_1X_CP|\
                                     TM_PRTL_TYPE_UMTS_UP|\
                                     TM_PRTL_TYPE_UMTS_CP|\
                                     TM_PRTL_TYPE_DIAG|\
                                     TM_PRTL_TYPE_PPM|\
                                     TM_PRTL_TYPE_GFC_QMI|\
                                     TM_PRTL_TYPE_XTRA_T | \
                                     TM_PRTL_TYPE_LPP_CP\
                                     )
/*--------------------------------------------------

   TM core session handle type for NI/MT sessions

---------------------------------------------------*/
typedef uint32 tm_sess_handle_type;
/* Pre-defined session handle for NI/MT sessions */
/* 0x0000 ~ 0x100 is reserved for MO/MI session */
#define TM_SESS_HANDLE_NONE          0x0000
#define TM_SESS_HANDLE_1X_CP_MT      0x0100
#define TM_SESS_HANDLE_1X_UP_NI      0x0200
#define TM_SESS_HANDLE_UMTS_CP_NI    0x0400
#define TM_SESS_HANDLE_UMTS_UP_NI    0x0800
#define TM_SESS_HANDLE_LPP_CP_NI     0x0801
#define TM_SESS_HANDLE_FTEST         0x1000 /* Used by diag module for scripted IS801 */
#define TM_SESS_HANDLE_FORCE_STOP    0x397f /* A magic handle to force session termination */
#define TM_SESS_HANDLE_ON_DEMAND     0x2000 /* Magic handle to identify on demand session */
#define TM_SESS_HANDLE_PERIODIC_PPM  0x3981 /* Magic handle to identify Periodic PPM sessions */
#define TM_SESS_HANDLE_E911          0x4000
/*------------------------------------------------------------------

   PD Comm transaction ID range allocation for protocol sub-moudles
    
--------------------------------------------------------------------*/
#define TM_1X_UP_TRANSACTION_ID_RANGE         0x01000000
/* 1x UP protocol sub-module is allowed to use transaction ID from 0x01000000 - 0x01FFFFFF*/

#define TM_UMTS_UP_TRANSACTION_ID_RANGE        0x02000000
/* UMTS UP protocol sub-module is allowed to use transaction ID from 0x02000000 - 0x02FFFFFF*/


/*------------------------------------------------------------------

   Goefence Manager transaction ID range allocation for protocol sub-moudles
    
--------------------------------------------------------------------*/
#define TM_UMTS_UP_GM_TRANSACTION_ID_BASE               0x01000000
#define TM_UMTS_UP_GM_TRANSACTION_ID_RANGE              0x00010000

/*--------------------------------------------------------------------

 Geofence Manager transaction ID range allocation for GFC_QMI
    
--------------------------------------------------------------------*/
#define TM_GFC_QMI_GM_TRANSACTION_ID_BASE               0x02000000
#define TM_GFC_QMI_GM_TRANSACTION_ID_RANGE              0x00010000

/*--------------------------------------------------------------------

 Geofence Manager transaction ID range allocation for TM_XTRA_T

--------------------------------------------------------------------*/
#define TM_XTRA_T_GM_TRANSACTION_ID_BASE               0x04000000
#define TM_XTRA_T_GM_TRANSACTION_ID_RANGE              0x00010000

/* UMTS UP protocol sub-module is allowed to use transaction ID from 0x02000000 - 0x02FFFFFF*/

/* Location Uncertainty */
#define TM_LR_THRESHOLD_NO_POS            6000000                /* 6000km no Pos available with MS */
#define TM_LR_THRESHOLD_NO_SEED_POS       5000                   /* 5km no pos for seed fix */
#define TM_TIME_THRESHOLD_NO_TIME         24000                  /* 24000 no ref time available with MS */
#define TM_PUNC_THRESHOLD_IN_SERVICE      12000                  /*  Use Punc threshold of 12km if 1X is in service */

#define TM_IS801_TIMEOUT                  35*1000   /* 35 secs in ms units */
#define TM_MGP_PPM_TIMEOUT_MAX            65*1000   /* PPM Guard timeout value for LM */
#define TM_MGP_GET_PPM_TIMEOUT_MAX        30*1000   /* Get PPM operation timeout */
#define TM_MGP_GET_PPM_TIMEOUT_MIN        500
#define TM_MGP_PR_TIMEOUT_MAX             30*1000

/* Target's max doppler uncertainty is 900 meters/sec */
#define TM_MGP_MAX_DOPPLER_UNC_M_SEC 900.0f

#if 0
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
#endif
/* threshold of num. of alm to be considered as complete constallation */
#define C_COMPLETE_CONSTALLATION_ALM_THRESHOLD  24
#define C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD  18

#define C_PI            3.1415926535898       /* From ICD */
#define C_GAD_LAT_TO_RAD (double)((double)C_PI / (double)16777216)      /* 90/2^23 * (pi/180) */
#define C_GAD_LON_TO_RAD (double)(2.0 * C_GAD_LAT_TO_RAD)               /* 360/2^24 * (pi/180) */

#define C_LAT_RAD_TO_GAD (double)( (double)16777216 / (double)C_PI )  /* (180/pi) * ( 2^23 / 90 ) */
#define C_LON_RAD_TO_GAD (double)( (double)8388608 / (double)C_PI )   /* (180/pi) * ( 2^24 / 360 ) */

/*--------------------------------------

       Phone Event Enumeration 
    
---------------------------------------*/
typedef enum
{
  TM_PHONE_EVENT_NONE = 0,
  TM_PHONE_EVENT_E911_CALL_ORIG,
  TM_PHONE_EVENT_IN_1X_COVERAGE,
  TM_PHONE_EVENT_IN_UMTS_COVERAGE,
  TM_PHONE_EVENT_OOS,
  TM_PHONE_EVENT_TC_CONNECTED,
  TM_PHONE_EVENT_TC_DISCONNECTED,
  TM_PHONE_EVENT_E911CB_ENTER,
  TM_PHONE_EVENT_E911CB_EXIT,
  TM_PHONE_EVENT_E911_CALL_END,
  TM_PHONE_EVENT_MT_MSG,
  TM_PHONE_EVENT_SENT_MSG_STATUS,
  TM_PHONE_EVENT_TC_EXIT,
  TM_PHONE_EVENT_TC_UP,
  TM_PHONE_EVENT_NEW_SS_INFO,
  TM_PHONE_EVENT_REVA_READ,
  TM_PHONE_EVENT_SS_SRV_DOMAIN,
  TM_PHONE_EVENT_SS_SRV_STATUS,
  TM_PHONE_EVENT_LCS_MOLR,
  TM_PHONE_EVENT_LCS_MOLR_CONF,
  TM_PHONE_EVENT_LCS_MOLR_COMPLETED,
  TM_PHONE_EVENT_LCS_MTLR_NTFY_IND,
  TM_PHONE_EVENT_LCS_MTLR_NTFY_RES,
  TM_PHONE_EVENT_SYS_MODE_LPM,
  TM_PHONE_EVENT_DSDS_SUBS_CHANGED,
  TM_PHONE_EVENT_OTASP_STATUS_COMMITTED,
  TM_PHONE_EVENT_IN_TDS_COVERAGE,  
  TM_PHONE_EVENT_HO_COMPLETE,
  TM_PHONE_EVENT_MAX
} tm_phone_event_e_type;

typedef enum
{
  TM_CM_STATUS_OK,                              /* Status ok */
  TM_CM_STATUS_FAILURE                          /* Status failure */
} tm_cm_sent_msg_status_e_type;

/*--------------------------------------

    Supported Positioning Protocols
    
---------------------------------------*/
typedef enum
{
  TM_POS_PRTL_NONE = 0,
  TM_POS_PRTL_RRLP,
  TM_POS_PRTL_IS801,
  TM_POS_PRTL_LPP,
  TM_POS_PRTL_LPPE,
  TM_POS_PRTL_RRC,
} tm_positioning_protocols;

typedef struct
{
  void *user_data;
  tm_cm_sent_msg_status_e_type msg_status;
} tm_cm_sent_msg_info_s_type;

typedef struct
{
  uint8 payload[TM_MAX_BUFFSIZE];                    /* Recv data buffer */
  uint16 length;                   /* The number of bytes in the payload */
} tm_cm_rcv_data_s_type;

#ifdef FEATURE_CGPS_UMTS_CP /* Needed as CM definitions are still under FEATURE_MMGPS */

typedef union
{
  location_notification_s_type location_notification_ind;
  lcs_molr_res_s_type lcs_molr_res;
} tm_cm_sups_data_u_type;

typedef struct
{
  uint8 u_ss_ref_num;
  uint8 u_invoke_id;
  tm_cm_sups_data_u_type lcs_mess;
} tm_cm_sups_data_s_type;
#endif /* FEATURE_CGPS_UMTS_CP */

typedef struct
{
  sys_srv_status_e_type srv_status;
  sys_sys_mode_e_type sys_mode;
  sys_srv_domain_e_type srv_domain;
  sys_plmn_id_s_type plmn;
  sys_sys_mode_e_type prev_sys_mode;
} tm_cm_ss_info_s_type;

typedef union
{
  tm_cm_rcv_data_s_type dbm_msg;
  tm_cm_sent_msg_info_s_type sent_msg_info;
  tm_cm_ss_info_s_type ss_info;
  #ifdef FEATURE_CGPS_UMTS_CP
  tm_cm_sups_data_s_type sups_data;
  #endif /* FEATURE_CGPS_UMTS_CP */
} tm_phone_event_payload_u_type;

typedef struct
{
  qword timestamp;             /* CDMA time that the data was received */
  int16 timestamp_msec;               /* Time in msec resolution 0-999 */
  byte msec_incl;                 /* Time in msec resolution included */
  uint16 time_ref_cdma;   /* CDMA system time at time solution is valid */
  sint31 latitude;                    /* Latitude, referenced to WGS-84 */
  sint31 longitude;                  /* Longitude, referenced to WGS-84 */
  byte loc_uncrtnty_ang;              /* Angle of axis wrt true north 
                                               for position uncertainty */
  byte loc_uncrtnty_a;      /* Standard deviation of axis along angle 
                                     specified for position uncertainty */
  byte loc_uncrtnty_p; /* Standard deviation of axis perpendicular to 
                               angle specified for position uncertainty */
  byte fix_type;                                          /* Fix type */
  byte velocity_incl;                /* Velocity information included */
  uint16 velocity_hor;                 /* Horizontal velocity magnitude */
  byte heading_valid;
  uint16 heading;                                            /* Heading */
  int1 velocity_ver;                   /* Vertical velocity magnitude */
  byte clock_incl;                      /* Clock information included */
  sint31 clock_bias;                                      /* Clock bias */
  sint15 clock_drift;                                    /* Clock drift */
  byte height_incl;                    /* Height information included */
  sint15 height;                        /* Height, referenced to WGS-84 */
  byte loc_uncrtnty_v;              /* Standard deviation of vertical 
                                       error for position uncertainty */

  pdsm_gnss_time_source_mask time_src;   /* src of timestamp is GPS/Glo? */
  FLT f_GGTBMs;                         /* GPS to Glonass bias (ms). */
  FLT f_GGTBUncMs;          /* GPS to Glonass bias uncertainty (ms). */
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/

  uint16 w_SensorDataUsageMask;    /* A bit set to 1 indicates that data from the corresponding 
                                      sensor was used in order to compute the content (position,
                                      velocity, heading) in this packet.
                                      0x0001 – Accelerometer
                                      0x0002 – Gyro
                                      0xFFFC - Reserved
                                      Future versions of the GPS service may use reserved bits 
                                      to represent new sensor types. A control point should 
                                      ignore any bits that it considers to be reserved. */
  uint16 w_SensorAidingIndicator;  /* A bit set to to 1 indicates that certain fields (position, 
                                      velocity, heading, etc) in parsed position data TLV were 
                                      aided with sensor data.
                                      0x0001 – Heading aided with sensor data
                                      0x0002 – Speed aided with sensor data
                                      0x0004 – Position aided with sensor data
                                      0x0008 – Velocity aided with sensor data
                                      0FFF0 – Reserved
                                      Future versions of the GPS service may use reserved bits 
                                      to represent new methods of sensor aiding (e.g. tightly 
                                      coupled measurement combinations). A control point should 
                                      ignore any bits that it considers to be reserved. */
  FLT f_clock_bias_unc;          /* Clock bias unc */
  uint8 u_TimeSource;            /* Extended Time source information */

  FLT   f_HeadingRad;     /* heading in rad. */
  FLT   f_HeadingUncRad;  /* heading uncertainty in rad. */
  FLT   f_GnssHeadingRad;     /* GNSS-only heading in rad. */
  FLT   f_GnssHeadingUncRad;  /* GNSS-only heading uncertainly in rad. */
  FLT   f_altitude; /* Floating point value of the altitude in meters */

  sm_GnssNavSvInfo   z_NavSvInfo;

  pdsm_pd_sensor_param_report   z_OnDeviceSensorParam;  /* On-device Sensor Bias data */
} tm_lr_resp_type;

typedef struct
{
  pdsm_pd_positioning_source_type positioning_source; /*gps/cellid/default */
  pdsm_pd_gps_position_e_type position_type; /* new/invalid */
  pdsm_pd_position_mode_e_type position_mode; /* MS BASED/MSASSISTED etc */
  uint32 gps_utc_offset;
} tm_pd_position_info_s_type;

typedef struct
{
  DBL ext_lat;                 /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
  DBL ext_lon;                 /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
  FLT ext_loc_uncrtnty_a;
  FLT ext_loc_uncrtnty_p;
  DBL ext_vel_horiz;
  DBL ext_vel_vert;
  FLT ext_vel_horiz_unc;
  FLT ext_vel_vert_unc;

  FLT h_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  FLT p_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  FLT v_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  pdsm_utc_time_s_type utc_time; /*  Utc time containing year,month,utc_days,hours,nin,secs,tenths*/
  FLT pos_hepe;           /*  The HEPE value in meters */
} tm_ext_pos_data_s_type;

/*-----------------------------------------------------------------------

                                MACROS    
-----------------------------------------------------------------------*/
#define SQR(f)  ((f)*(f))


/*-----------------------------------------------------------------------

                           Extern Functions
    
-----------------------------------------------------------------------*/


/*===========================================================================
FUNCTION tm_util_select_pos_unc

DESCRIPTION
    tm_util_select_pos_unc() Encodes a Standard deviation for position 
    uncertainty value into an integer based on table 3.2.4.2-10 of the 
    IS-801 spec.

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
extern int16 tm_util_select_pos_unc( float Std );

/*===========================================================================
FUNCTION tm_util_nearest_long

DESCRIPTION
    tm_util_nearest_long() rounds the double floating point var and returns long

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
extern int32 tm_util_nearest_long( double d_X );


/*===========================================================================

FUNCTION TM_UTIL_PD_INFO_DATA_INIT

DESCRIPTION
  This function is used to initialize the PD realted data for sending
  it to clients.

RETURN VALUE
  Events to be set for Client's.

DEPENDENCIES

===========================================================================*/
extern pdsm_pd_event_type tm_util_pd_info_data_init
(
   pdsm_pd_info_s_type *cl_info,
   const tm_lr_resp_type *lr_ptr,
   pdsm_client_id_type client_id,
   pdsm_pd_dload_data_s_type *pd_dload_info_ptr,
   tm_pd_position_info_s_type *pd_pos_info_ptr,
   tm_ext_pos_data_s_type *ext_pos_data_ptr
   );

/*============================================================================
FUNCTION tm_util_convert_lr

DESCRIPTION
    Function to convert navsolution into IS801 Location response structure

RETURN VALUE
    None

DEPENDENCIES

LIMITATIONS

=============================================================================*/
extern boolean tm_util_convert_lr
(
  sm_GnssFixRptStructType *input_lr_buf_ptr,
  tm_lr_resp_type         *output_lr_buf_ptr
);

/*===========================================================================
FUNCTION tm_util_post_non_exist_svs

DESCRIPTION

  This function is used by all protocol submodules when the submodule is 
  convinced it has received all the assistance data it can get in the 
  current session, to:

  (1) Determine if it is in a position to deduce which SVs do not exist;
  (2) If the answer to (1) is positive, post such info to TM core; if the
      answer to (1) is negative, no further action will be taken.

DEPENDENCIES 
  None.

PARAMETERS:
  
  q_prtl_type:     of tm_prtl_type as defined in tm_common.h.
  session_handle:  of tm_session_handle_type as defined in tm_common.h.
  q_alm_list:      bit map of alm. delivered; SV1 on Bit 0.
  q_eph_list:      bit map of eph. delivered; SV1 on Bit 0

RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

extern void tm_util_post_non_exist_svs
( tm_prtl_type q_prtl_type,
  tm_sess_handle_type session_handle,
  uint32 q_alm_list,
  uint32 q_eph_list
  );

/*===========================================================================
FUNCTION tm_util_post_non_exist_glo_svs

DESCRIPTION

  This function is used by all protocol sub-modules to inject Glonass non-existence SVs
  assitance data, when it has received all the Glonass assistance data it can get in the
  current session, to:

  (1) Determine if it is in a position to deduce which Glonass SVs do not exist;
  (2) If the answer to (1) is positive, post such info to TM core; if the
      answer to (1) is negative, no further action will be taken.
 
  GNSS-engine supports injecting Glonass non-existence SVs not non-exist frequency.
 
DEPENDENCIES 
  None.

PARAMETERS:
  
  q_prtl_type:     of tm_prtl_type as defined in tm_common.h.
  session_handle:  of tm_session_handle_type as defined in tm_common.h.
  q_alm_list:      bit map of alm. delivered; SV1 on Bit 0.
  q_eph_list:      bit map of eph. delivered; SV1 on Bit 0
  p_rrc_glo_eph_freq: Glonass SV/Slot-Id to frequency number mapping info.
 
RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

extern void tm_util_post_non_exist_glo_svs( tm_prtl_type q_prtl_type, tm_sess_handle_type session_handle,
                                            uint32 q_glo_alm_list, uint32 q_glo_eph_list ); 


/*===========================================================================
FUNCTION tm_util_calculate_heading

DESCRIPTION

  This function is used to calculate the heading value from the PE position report

DEPENDENCIES 
  None.

PARAMETERS:
  sm_GpsFixRptStructType  *fix_ptr,
  Heading 
  

RETURN VALUE
    
SIDE EFFECTS
  None

===========================================================================*/

extern void tm_util_calculate_heading
( const sm_GnssFixRptStructType *fix_ptr,
  DBL *heading_angle
  );

/*
 ******************************************************************************
 *
 * tm_util_get_cep_scale_ratio_from_pct
 *
 * Function description:
 *  This function gives the ratio to scale circular uncertainties (CEP's) from
 *  different percentiles.  E.g. 39% conf. "standard" to 99% conf.
 *
 * Parameters:
 *
 *  f_from_percentile: CEP percentile to convert from (range 0.00 to 1.00)
 *  f_to_percentile: CEP percentile to convert to (range 0.00 to 1.00)
 *
 * Return value:
 *
 *  The scale factor to multiply by to scale the CEP<from>% to a CEP<to>% 
 *   value (in meters.)
 *
 ******************************************************************************
*/
extern float tm_util_get_cep_scale_ratio_from_pct( float f_from_percentile,
                                                   float f_to_percentile );


/**
 * @brief Converts calendar time with milliseconds into UTC time structure.
 *
 * During the conversion the milliseconds are truncated to fit the storage type.
 *
 * @param[in]  pz_TimeIn  Calendar time with milliseconds.
 * @param[out] pz_TimeOut UTC time structure.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_time_to_utc_time
(
  const tm_CalendarTimeT *pz_TimeIn,
  utc_time               *pz_TimeOut
);

/**
 * @brief Converts calendar time with milliseconds into UTC time structure.
 *
 * During the conversion the milliseconds are truncated to fit the storage type.
 *
 * @param[in]  pz_TimeIn  Calendar time with milliseconds.
 * @param[out] pz_TimeOut UTC time structure.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_time_to_pdsm_time
(
  const tm_CalendarTimeT *pz_TimeIn,
  pdsm_utc_time_s_type   *pz_TimeOut
);

/**
 * @brief Constructs calendar structure from GPS input.
 *
 * @param[in]  w_GpsWeek        GPS week number.
 * @param[in]  q_FixTimeGpsMsec GPS fix time in milliseconds.
 * @param[in]  q_FixTimeUtcMsec UTC fix time in milliseconds.
 * @param[out] pz_TimeOut       Conversion result.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @sa tm_util_construct_glo_time
 */
boolean tm_util_construct_gps_time
(
  uint16            w_GpsWeek,
  uint32            q_FixTimeGpsMsec,
  uint32            q_FixTimeUtcMsec,
  tm_CalendarTimeT *pz_TimeOut
);

/**
 * @brief Constructs calendar structure from GLO input.
 *
 * @param[in]  u_GloNumFourYear       GLO 4 year number from 1996.
 * @param[in]  w_GloNumDaysInFourYear GLO day number in 4 years.
 * @param[in]  q_GloTimeMs            GLO millisecond in the day.
 * @param[out] pz_TimeOut             Conversion result.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @sa tm_util_construct_gps_time
 */
boolean tm_util_construct_glo_time
(
  uint8             u_GloNumFourYear,
  uint16            w_GloNumDaysInFourYear,
  uint32            q_GloTimeMs,
  tm_CalendarTimeT *pz_TimeOut
);

/**
 * @brief Construct UTC time from GPS time fields.
 *
 * This function constructs UTC time YearMonthHourMinuteSecond using GPS
 * WeekNumber, GPS msec_of_week and UTC leap time adjusted msec_of_week.
 *
 * @param[in] w_FixWn          fix time GPS week;
 * @param[in] q_FixTimeGpsMsec fix time GPS msec. of the week;
 * @param[in] q_FixTimeUtcMsec fix time UTC msec (adj. for leap seconds ).
 *
 * @return UTC time YearMonthHourMinuteSecond. (if fails to construct UTC time,
 *         UTC will be set to all zero)
 */
utc_time tm_util_construct_utc_time
(
  uint16 w_FixWn,
  uint32 q_FixTimeGpsMsec,
  uint32 q_FixTimeUtcMsec
);

/*===========================================================================
FUNCTION tm_util_efs_dir_and_file_name_is_valid

DESCRIPTION
  This function verifies if the directory and file name is one of the valid names
  defined in acceptable list
  The following fields are checked
  - if the filenamelength is >  PDSM_PD_FS_MAX_PATH_LEN_BYTES returns an error
  - if the file length is > PDSM_PD_EFS_MAX_FILE_LEN_BYTES  returns an error
  - if the file operation is not WRITE or DELETE, returns an error
  - if the name does not match any of the names in the valid list, returns an error
  

DEPENDENCIES
  None.

RETURN VALUE
   TRUE - Valid directory and file name
   FALSE - Error, Invalid directory and file name

SIDE EFFECTS
  None.
===========================================================================*/

boolean tm_util_efs_dir_and_file_name_is_valid( const char *p_dir_and_filename,
                                                uint8 file_operation,
                                                uint32 file_size );


/*===========================================================================
FUNCTION tm_util_post_unhealthy_svs

DESCRIPTION

  This function is used by all protocol submodules when the submodule is 
  convinced it has received all the ephemeris data it can get in the 
  current session. If delivered list of Ephemeris for SVs is not the same
  as the requested list, the remaining SVs are marked unhealthy and sent
  to PE:


DEPENDENCIES 
  None.

PARAMETERS:
  
  q_prtl_type:     of tm_prtl_type as defined in tm_common.h.
  session_handle:  of tm_session_handle_type as defined in tm_common.h.
  q_eph_req_list:  bit map of eph. requested; SV1 on Bit 0.
  q_eph_delivered_list:      bit map of eph. delivered; SV1 on Bit 0

RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/

extern void tm_util_post_unhealthy_svs
( tm_prtl_type q_prtl_type,
  tm_sess_handle_type session_handle,
  uint32 q_eph_unhealthy_list
  );

/*
 ******************************************************************************
 *

FUNCTION TM_UTIL_XLATE_NAV_SOLN_TO_TM_POS_INFO

DESCRIPTION
  This function is used to translate sm_GnssNavPosition strucutre to 
  tm_ext_pos_data_s_type structure

RETURN VALUE

DEPENDENCIES
 ******************************************************************************
*/

void tm_util_xlate_nav_soln_to_tm_pos_info( pdsm_pd_dload_data_s_type*  dload_info,
                                            tm_pd_position_info_s_type* pos_info,
                                            const sm_GnssNavPosition*   nav_ptr,
                                            pdsm_pd_dload_e_type        dload_type,
                                            pdsm_pd_dload_source_e_type dload_src
                                          );

/*===========================================================================

FUNCTION TM_UTIL_XLATE_NAV_SOLN_TO_TM_EXT_POS

DESCRIPTION
  This function is used to translate sm_GnssNavPosition structure to 
tm_ext_pos_data_s_type structure

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void tm_util_xlate_nav_soln_to_tm_ext_pos( tm_ext_pos_data_s_type*   ext_pos_data_ptr,
                                           const sm_GnssNavPosition* nav_ptr); 

/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long
 *
 * Function description:
 *  This function counts the number of bits set in a uint32 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  q_long_word: bit mask;
 *
 * Return value:
 *
 * number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long( uint32 q_long_word );

/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long64
 *
 * Function description:
 *  This function counts the number of bits set in a uint64 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  t_long [IN]: bit mask;
 *
 * Return value:
 *
 *  Number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long64( uint64 t_long );

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_horiz_uncertainty
 *
 * Function description:
 *  This function calculate the horizontal velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  horizontal velocity uncertainty (m/s)
 ******************************************************************************
*/
extern FLT tm_util_calc_vel_horiz_uncertainty(const sm_GnssNavPosition* nav_ptr);

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_vert_uncertainty
 *
 * Function description:
 *  This function calculate the vertical velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  vertical velocity uncertainty (m/s)
 ******************************************************************************
*/
extern FLT tm_util_calc_vel_vert_uncertainty(const sm_GnssNavPosition* nav_ptr);

/**
 * @brief Computes UTC time from GPS or GLO time source.
 *
 * This function calculates UTC time YearMonthHourMinuteSecond using GPS
 * time or GLONASS time specified in fix report. If both GPS and GLONASS
 * time are valid, GPS time will be used to calculate UTC.
 *
 * This routine can return UTC in two different UTC formats (pdsm_utc_time
 * and utc_time), you can set one to NULL if not used. If you pass in both
 * pointers, two UTC formats will be returned.
 *
 * @param[in]  pz_Fix         Fix report pointer
 * @param[out] pz_PdsmTimeOut PDSM UTC structure pointer (optional, can be NULL)
 * @param[out] pz_UtcTimeOut  TM UTC structure pointer (optional, can be NULL)
 *
 * @retval TRUE  Successfully calculated UTC time
 * @retval FALSE Failed to calculate UTC time (UTC will be set to all zero)
 *
 * @deprecated Use tm_util_calculate_time, tm_util_time_to_utc_time,
 *             tm_util_time_to_pdsm_time
 *
 * @sa tm_util_calculate_time
 * @sa tm_util_time_to_utc_time
 * @sa tm_util_time_to_pdsm_time
 */
boolean tm_util_calculate_utc
(
  const sm_GnssFixRptStructType *pz_Fix,
  pdsm_utc_time_s_type          *pz_PdsmTimeOut,
  utc_time                      *pz_UtcTimeOut
);

/**
 * @brief Obtains calendar time from fix report.
 *
 * This function calculates Gregorian calendar time with millisecond accuracy
 * using GPS or GLONASS time specified in fix report. If both GPS and GLONASS
 * time fields are valid, GPS time will be used to calculate UTC.
 *
 * @param[in]  pz_Fix     Fix report.
 * @param[out] pz_TimeOut Computed values.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_get_fix_time
(
  const sm_GnssFixRptStructType *pz_Fix,
  tm_CalendarTimeT              *pz_TimeOut
);

/*
******************************************************************************
*
 * FUNCTION:
 * tm_util_calculate_position_print
 *
 * DESCRIPTION:
 *  This function calculates position using fix report
 *  Position is calcuated in Latitude and Longitude and printed to the i/p string
 *
 * PARAMETERS:
 *  p_fix:          [Input] ptr to fix report
 *  p_Str:          [Output]ptr to the string where data will be written
 *  q_buf_len:      length of string
 *
 * RETURN VALUE:
 *  q_len           length of string written
 *
 ******************************************************************************
*/
extern int tm_util_calculate_position_print
( const sm_GnssFixRptStructType *p_fix, 
  char *p_Str, 
  int q_buf_len
);

/**
 * @brief Converts Glonass time to UTC time.
 *
 * @param[in]  u_GloNumFourYear       Count of 4-year intervals since 1996.
 * @param[in]  w_GloNumDaysInFourYear Number of days within 4-year interval.
 * @param[in]  q_GloTimeMs            Number of milliseconds since midnight.
 * @param[out] pz_UtcTimeOut          Parsed UTC structure with decisecond
 *                                    accuracy.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @deprecated Use tm_util_construct_glo_time
 * @sa tm_util_construct_glo_time
 */
boolean tm_util_glo_to_utc
(
  uint8 u_GloNumFourYear,
  uint16 w_GloNumDaysInFourYear,
  uint32 q_GloTimeMs,
  utc_time* pz_UtcTimeOut
);

/*===========================================================================
FUNCTION tm_core_get_gnss_emergency_protocol_non_lte

DESCRIPTION
  This function determines if Control Plane or UP protocol is used for E911 on non-LTE 

 

RETURN VALUE
  TRUE if UP is enabled, FALSE if CP

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
uint32 tm_core_get_gnss_emergency_protocol_non_lte( void );


/*===========================================================================

FUNCTION tm_core_send_pdapi_emerg_sess_type

DESCRIPTION
  This function handles send a PDAPI event to the clients of the type of emergency session
  in progress
  
DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/

extern void tm_core_send_pdapi_emerg_sess_type(pdsm_ext_status_emerg_sess_e_type sess_type);

/**
 * @brief Adjusts time information according to GPS offset time (deltaT)
 *
 * This function processes GPS offset time (deltaT) to compute the GLONASS time
 * & set the appropriate GLONASS time fields in gnss_time_info struct.
 * The deltaT could be from GPS UTCModel AD and/or GLONASS TimeModel AD.
 *
 * @param[in]     b_DeltaT     Positive GPS time offset value.
 * @param[in,out] pz_AgnssTime Pointer to position time structure.
 * 
 * @retval TRUE  On success
 * @retval FALSE Failure indication.
 */
boolean tm_util_convert_to_glonass_time
(
  int8            b_DeltaT,
  gnss_time_info *pz_AgnssTime
);

/*
 ******************************************************************************
 *
 * tm_util_get_rand
 *
 * Function description:
 *   Gets a random number from security module.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *  uint32 random number.
 *
 ******************************************************************************
*/
uint32 tm_util_get_rand(void);

/*
 ******************************************************************************
 *
 * tm_util_clean_qzss_sbas_meas_rpt
 *
 * Function description:
 *   Remove either SBAS or QZSS information from meas block which source 
 *   is GNSS_MEAS_BLK_SRC_QZSS_SBAS. 
 *
 * Parameters:
 *   p_measReport [IN/OUT]: QZSS/SBAS Measurement report structure
 *   v_keep_sbas  [IN]:     TRUE  - Remove QZSS measurements
 *                          FALSE - Remove SBAS measurements
 * Return value:
 *  None
 *
 ******************************************************************************/

void tm_util_clean_qzss_sbas_meas_rpt(sm_GpsMeasRptStructType* p_measReport,boolean v_keep_sbas);


#ifdef __cplusplus
}
#endif


#endif /* TM_COMMON_H */
