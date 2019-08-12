
/******************************************************************************
  @file: loc_api_2.h
  @brief: Location API header file, Major verion 2

  DESCRIPTION
    Qualcommm Location API header 2.0

  INITIALIZATION AND SEQUENCING REQUIREMENTS
    N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
01/11/16   yh       Loc_Wifi cleanup
10/05/15   yh       QZSS
06/11/15   sj       LB 2.0 Integration
05/18/15   jv       Added deletion bits for SBAS eph & alm.
09/05/14   ssu     GM LOWI Integration
08/21/14   jv       Added support for 16-bit SVID  
04/15/14   yh       XTRA version check added  
03/10/14   ah       Added support for Set Premium Services Cfg & Get Avail WWAN Pos
01/24/14   ah       Added GLO support for Get/Set NMEA Type Requests in Loc MW
02/25/13   ssu      Adding a new WWAN type used for AGNSS Emergency connections. 
07/14/11   ly       Initial version

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/inc/loc_api_2.h#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#ifndef LOC_API_2_H
#define LOC_API_2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"    /* Definition for basic types and macros */
#include "pdapi.h"
#include "location_service_v02.h"

/*=============================================================================
 *
 *                             DATA DECLARATION
 *
 *============================================================================*/


/******************************************************************************
 *  Constants and configuration
 *****************************************************************************/
#define LOC_API_MAJOR_VERSION_NUMBER   2
#define LOC_API_MINOR_VERSION_NUMBER   3

/* Return value for loc_open in case of failure. */
#define LOC_CLIENT_HANDLE_INVALID     -1

/* Return value of loc api calls for loc_close, loc_start_fix, loc_stop_fix 
 * and loc_ioctl
 * These are also the status for the ioctl callback */
#define LOC_API_SUCCESS                     (0)
#define LOC_API_GENERAL_FAILURE             (1)
#define LOC_API_UNSUPPORTED                 (2)
#define LOC_API_INVALID_HANDLE              (4)
#define LOC_API_INVALID_PARAMETER           (5)
#define LOC_API_ENGINE_BUSY                 (6)
#define LOC_API_PHONE_OFFLINE               (7)
#define LOC_API_TIMEOUT                     (8)
#define LOC_API_XTRA_VERSION_CHECK_FAILURE  (9)

/* Max Count lengths for various data structures */
#define LOC_API_MAX_SV_COUNT                      80
#define LOC_API_MAX_NMEA_STRING_LENGTH            200

/* Maximum server address that will be used in location API */
#define LOC_API_MAX_SERVER_ADDR_LENGTH            256
#define LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS  3
#define LOC_API_MAX_NUM_NTP_SERVERS               3

/******************************************************************************
 *  Misc global data
 *****************************************************************************/
/* Client handle that will be returned to LocAPI Clients */
typedef int32 loc_client_handle_type;

/******************************************************************************
 *  Location callback event masks
 *****************************************************************************/
typedef uint64 loc_event_mask_type;
#define LOC_EVENT_PARSED_POSITION_REPORT             0x00000001 
              /* Position report comes in loc_parsed_position_s_type */
#define LOC_EVENT_SATELLITE_REPORT                   0x00000002 
              /* Satellite in view report */
#define LOC_EVENT_NMEA_1HZ_REPORT                    0x00000004 
             /* NMEA report at 1HZ rate */
#define LOC_EVENT_NMEA_POSITION_REPORT               0x00000008 
             /* NMEA report at position report rate */
#define LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST           0x00000010 
             /* NI notification/verification request */
#define LOC_EVENT_ASSISTANCE_DATA_REQUEST            0x00000020 
             /* Assistance data, eg: time, predicted orbits request */
#define LOC_EVENT_LOCATION_SERVER_REQUEST            0x00000040 
             /* Request for location server */
#define LOC_EVENT_IOCTL_REPORT                       0x00000080 
             /* Callback report for loc_ioctl */
#define LOC_EVENT_STATUS_REPORT                      0x00000100 
            /* Misc status report: eg, engine state */
#define LOC_EVENT_WPS_NEEDED_REQUEST                 0x00000200 
            /* Request to start/stop Wifi fixes */
#define LOC_EVENT_SENSOR_STREAMING_READY_STATUS      0x00000400
            /* Request to listenn to sensor Streaming Status */
#define LOC_EVENT_TIME_SYNC_REQ                      0x00000800
            /* Request to listen for Time Sync Requests */
#define LOC_EVENT_SPI_STREAMING_REPORT               0x00001000 

            /* Request for geofence batch breach reports  */
#define LOC_EVENT_GEOFENCE_BATCH_BREACH_REPORT       0x00002000 
#define LOC_EVENT_GNSS_MEASUREMENT_REPORT            0x01000000
#define LOC_EVENT_SV_POLY_REPORT                     0x02000000

/* Request to listen for SPI STreaming reports */
/* Reserved event for internal development, not exposed to open source */
#define LOC_EVENT_RESERVED                           0x8000000000000000ULL

/*=============================================================================
 *
 *           Location data types for API and callback functions
 *
 *============================================================================*/

/******************************************************************************
 *  For LOC_EVENT_PARSED_POSITION_REPORT
 *****************************************************************************/


typedef uint64 loc_position_valid_mask_type;
#define LOC_POS_VALID_SESSION_STATUS                 0x00000001
#define LOC_POS_VALID_TIMESTAMP_CALENDAR             0x00000002
#define LOC_POS_VALID_TIMESTAMP_UTC                  0x00000004
#define LOC_POS_VALID_LEAP_SECONDS                   0x00000008
#define LOC_POS_VALID_TIME_UNC                       0x00000010
#define LOC_POS_VALID_LATITUDE                       0x00000020
#define LOC_POS_VALID_LONGITUDE                      0x00000040
#define LOC_POS_VALID_ALTITUDE_WRT_ELLIPSOID         0x00000080
#define LOC_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL    0x00000100
#define LOC_POS_VALID_SPEED_HORIZONTAL               0x00000200
#define LOC_POS_VALID_SPEED_VERTICAL                 0x00000400
#define LOC_POS_VALID_HEADING                        0x00000800
#define LOC_POS_VALID_HOR_UNC_CIRCULAR               0x00001000
#define LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MAJ          0x00002000
#define LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MIN          0x00004000
#define LOC_POS_VALID_HOR_UNC_ELLI_ORIENT_AZIMUTH    0x00008000
#define LOC_POS_VALID_VERTICAL_UNC                   0x00010000
#define LOC_POS_VALID_SPEED_UNC                      0x00020000
#define LOC_POS_VALID_HEADING_UNC                    0x00040000
#define LOC_POS_VALID_CONFIDENCE_HORIZONTAL          0x00080000
#define LOC_POS_VALID_CONFIDENCE_VERTICAL            0x00100000
#define LOC_POS_VALID_MAGNETIC_VARIATION             0x00200000
#define LOC_POS_VALID_TECHNOLOGY_MASK                0x00400000
#define LOC_POS_VALID_SENSOR_AIDING_IND              0x00800000
#define LOC_POS_VALID_SENSOR_DATA_USAGE              0x01000000
#define LOC_POS_VALID_H_RELIABILITY_IND              0x02000000
#define LOC_POS_VALID_V_RELIABILITY_IND              0x04000000
#define LOC_POS_VALID_TIME_SOURCE                    0x08000000
#define LOC_POS_VALID_EXT_TIME_SOURCE                0x10000000
#define LOC_POS_VALID_DOP                            0x20000000
#define LOC_POS_VALID_SV_USED_MASK                   0x40000000
#define LOC_POS_VALID_ALTITUDE_ASSUMED_MASK          0x80000000
#define LOC_POS_VALID_SPEED_ENU                     0x100000000

/* defaults used on location engine */
#define LOC_DEFAULT_RESPONSE_TIME                (255*1000) /* 255 seconds */
#define LOC_MIN_RESPONSE_TIME                    (1000)     /* 1 second */
#define LOC_CP_MO_RESPONSE_TIME                  (30*1000)  /* 30 seconds */

/*  Conversions of Accuracy Levels from QMI-Loc Levels to Meters */
#define LOC_ACCURACY_HIGH      (50)
#define LOC_ACCURACY_MED       (500)
#define LOC_ACCURACY_LOW       (1000)
#define LOC_DEFAULT_ACCURACY   LOC_ACCURACY_LOW  /* Accuracy is LOW by default */

typedef enum
{
  LOC_SESS_STATUS_SUCCESS    = 0,      /* Fix has been computed successfully */
  LOC_SESS_STATUS_IN_PROGESS = 1,      /* Intermediate position report */
  LOC_SESS_STATUS_GENERAL_FAILURE = 2, /* Fix has failed for unknown reason */
  LOC_SESS_STATUS_TIMEOUT    = 3,      /* Fix has failed with particular reason */
  LOC_SESS_STATUS_USER_END   = 4,      /* Session stopped by loc_stop_fix */
  LOC_SESS_STATUS_BAD_PARAMETER = 5,   /* Bad parameter */
  LOC_SESS_STATUS_PHONE_OFFLINE  = 6,  /* Phone offline */
  LOC_SESS_STATUS_ENGINE_LOCKED  = 7, /* Location engine is locked */
  LOC_SESS_STATUS_MAX = 0x10000000     /* used to force enum to 32-bit */
} loc_session_status_e_type;

typedef struct
{
   uint16        year;
   unsigned char month;
   unsigned char day_of_week;
   unsigned char day;
   unsigned char hour;
   unsigned char minute;
   unsigned char second;
   uint16        millisecond;
} loc_calendar_time_s_type;

typedef struct
{
   loc_position_valid_mask_type      valid_mask;
   loc_session_status_e_type         session_status;                /* 0: no error */
   loc_calendar_time_s_type          timestamp_calendar;            /* Calendar time stamp */
   uint64                            timestamp_utc;                 /* UTC timestamp in millisecond since Jan 1st, 1970 */
   /* If leap seconds info is not available, timestamp_utc is calcuated based on hard-coded value */
   uint8                             leap_seconds;                  /* Leap second information, in seconds unit */
   float                             time_unc;                      /* Time uncertainty, in millisecond unit*/
   double                            latitude;                      /* In decimal degrees. Range [-90.0, 90.0]. Positive values indicate northern latitude
                                                                       and negative values southern latitude.  */
   double                            longitude;                     /* In decimal degrees.  [-180.0, 180.0) Positive values indicate eastern longitude and negative values
                                                                       western longitude. The longitude is given in WGS84 datum. */
   float                             altitude_wrt_ellipsoid;        /* In meters above WGS84 ellipsoid */
   float                             altitude_wrt_mean_sea_level;   /* In meters above mean sea level */
   float                             speed_horizontal;              /* In meters/second */
   float                             speed_vertical;                /* In meters/second */
   float                             heading;                       /* In degrees 0 to 359.999  */
   float                             hor_unc_circular;              /* In meters */
   float                             hor_unc_ellipse_semi_major;    /* In meters */
   float                             hor_unc_ellipse_semi_minor;    /* In meters */
   float                             hor_unc_ellipse_orient_azimuth; /* Defines orientation of the uncertainty ellipse,
                                                                        in azimuth (decimal degrees 0-180) */
   float                             vert_unc;                      /* Meters */
   float                             speed_unc;                     /* In meters */
   float                             heading_unc;                   /* In degree */

   unsigned char                     confidence_horizontal;         /* 0-99 */
   unsigned char                     confidence_vertical;           /* 0-99 */

   float                             magnetic_deviation;   /* Difference between the 
             bearing to true north and the bearing shown on a magnetic compass.
             The declination is positive when the magnetic north is east of 
             true north. */
             
   float                            f_VelocityENUMps[3];          /* East, North, Up velocity (m/s) */
   float                            f_VelocityUncENUMps[3];    /* East, North, Up velocity uncertainty (m/s) */

  qmiLocPosTechMaskT_v02             technology_mask;  /* Technologies being used in computing this fix */

   uint16                            sensor_aiding_indicator; /* This is a bit mask. A bit set to to 1
                                     indicates that corresponding field 
                                     (position, velocity, heading, etc) in 
                                     fix report was aided with sensor data.
                                     0x0001 – Heading aided with sensor data
                                     0x0002 – Speed aided with sensor data
                                     0x0004 – Position aided with sensor data
                                     0x0008 – Velocity aided with sensor data
                                     0FFF0 – Reserved
                                     Future versions of the GPS service may use 
                                     reserved bits to represent new methods of 
                                     sensor aiding (e.g. tightly coupled 
                                     measurement combinations). A PDAPI client
                                     should ignore any bits that it considers 
                                     to be reserved. */

   uint16                           sensor_data_usage_mask; /* This is a bit mask.
                                       A bit set to 1 indicates that data from
                                       the corresponding sensor was used in order
                                       to compute the content(position, velocity, 
                                       heading) in the fix report.
                                       0x0001 – Accelerometer
                                       0x0002 – Gyro
                                       0xFFFC - Reserved
                                       Future versions of the GPS service may use
                                       reserved bits to represent new sensor 
                                       types. A PDAPI client should ignore any 
                                       bits that it considers to be reserved. */

   pdsm_pd_reliability_e_type       hor_reliability;    /*horizontal reliability*/
   pdsm_pd_reliability_e_type       vert_reliability;    /*vertical reliability*/
   pdsm_gnss_time_source_mask       time_soruce;         /* source of gnss time */
   pdsm_extended_time_src_e_type    extended_time_source;    /* Extended Time source information */
   float                            position_dop;        /* PDOP = sqrt(HDOP^2 +VDOP^2) */
   float                            hor_dop;      /* unitless */
   float                            vert_dop;        /* unitless */
   boolean                          altitude_assumed; /* a valid altitude is either assumed or calculated */

   /* for SV Info */
   pdsm_GnssNavSvInfo z_NavSvInfo;

} loc_parsed_position_s_type;

typedef struct
{
   
   /* for SV Info */
   pdsm_pd_ext_gnss_meas_s_type z_NavGnssMeas;

} loc_gnss_meas_s_type;

typedef struct
{
   
   /* for SV Poly */
   pdsm_pd_ext_SvPoly_s_type z_SvPoly;

} loc_sv_poly_s_type;

/******************************************************************************
 * For LOC_EVENT_SATELLITE_REPORT
 * Report those SVs that are currently in view
 *****************************************************************************/

typedef enum
{
  LOC_SV_SYSTEM_GPS     = 1,
  LOC_SV_SYSTEM_GALILEO = 2,
  LOC_SV_SYSTEM_SBAS    = 3,
  LOC_SV_SYSTEM_COMPASS = 4,
  LOC_SV_SYSTEM_GLONASS = 5,
  LOC_SV_SYSTEM_BDS     = 6,
  LOC_SV_SYSTEM_QZSS    = 7,
  LOC_SV_SYSTEM_MAX = 0x10000000 /* Make enum 32 bit */
} loc_sv_system_e_type;

typedef enum
{
  LOC_SV_STATUS_IDLE   = 1,  /* SV is not being actively processed */
  LOC_SV_STATUS_SEARCH = 2,
  LOC_SV_STATUS_TRACK  = 3,
  LOC_SV_STATUS_MAX    = 0x10000000 /* Make enum 32 bit */
} loc_sv_status_e_type;

typedef uint32 loc_sv_info_valid_mask_type;
#define LOC_SV_INFO_VALID_SYSTEM         0x00000001
#define LOC_SV_INFO_VALID_PRN            0x00000002
#define LOC_SV_INFO_VALID_HEALTH_STATUS  0x00000004
#define LOC_SV_INFO_VALID_PROCESS_STATUS 0x00000008
#define LOC_SV_INFO_VALID_HAS_EPH        0x00000010
#define LOC_SV_INFO_VALID_HAS_ALM        0x00000020
#define LOC_SV_INFO_VALID_ELEVATION      0x00000040
#define LOC_SV_INFO_VALID_AZIMUTH        0x00000080
#define LOC_SV_INFO_VALID_SNR            0x00000100

typedef struct
{
  loc_sv_info_valid_mask_type valid_mask;
  loc_sv_system_e_type        system;         /* The system that this SV belongs to */
  uint16                      prn;            /* Id of the sv */
  uint8                       health_status;  /* SV healthy level, 0 - unhealthy, 1 - healthy */
  loc_sv_status_e_type        process_status; /* Idle, search, track */
  boolean                     has_eph;        /* Do we have eph for this sv */
  boolean                     has_alm;        /* Do we have alm for this sv */
  float                       elevation;      /* In degrees, 0-90.0 */
  float                       azimuth;        /* In degrees, 0-360.0 */
  float                       snr;            /* SNR in dBHz units */
} loc_sv_info_s_type;

typedef uint32 loc_gnss_info_valid_mask_type;

#define LOC_GNSS_INFO_VALID_POS_DOP           0x00000001
#define LOC_GNSS_INFO_VALID_HOR_DOP           0x00000002
#define LOC_GNSS_INFO_VALID_VERT_DOP          0x00000004
#define LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED  0x00000008
#define LOC_GNSS_INFO_VALID_SV_COUNT          0x00000010
#define LOC_GNSS_INFO_VALID_SV_LIST           0x00000020

typedef struct
{
  loc_gnss_info_valid_mask_type      valid_mask;
  float                              position_dop;     /* PDOP = sqrt(HDOP^2 +VDOP^2) */
  float                              horizontal_dop;   /* unitless */
  float                              vertical_dop;     /* unitless */
  boolean                            altitude_assumed; /* a valid altitude is either assumed or calculated */
  uint16                             sv_count;         /* number of SVs to be parsed */
  loc_sv_info_s_type                 sv_list[LOC_API_MAX_SV_COUNT];
} loc_gnss_info_s_type;
/*~ FIELD loc_gnss_info_s_type.sv_list
    VARRAY LOC_API_MAX_SV_COUNT
    LENGTH loc_gnss_info_s_type.sv_count
 */


/******************************************************************************
 *  For LOC_EVENT_NMEA_1HZ_REPORT & LOC_EVENT_NMEA_POSITION_REPORT
 *****************************************************************************/
typedef struct
{
   uint16 length; /* actual size of the nmea string */
   char   nmea_sentences[LOC_API_MAX_NMEA_STRING_LENGTH];
} loc_nmea_report_s_type;
/*~ FIELD loc_nmea_report_s_type.nmea_sentences
    VARRAY (loc_nmea_report_s_type.length + 1) */

/******************************************************************************
 *  For LOC_EVENT_STATUS_REPORT
 *****************************************************************************/
typedef enum
{
   LOC_STATUS_EVENT_ENGINE_STATE = 1, /* GNSS engine state */
   LOC_STATUS_EVENT_FIX_SESSION_STATE = 2, /* Fix session state */
   LOC_STATUS_EVENT_MAX = 0x10000000  /* used to force enum to 32-bit */
} loc_status_event_e_type;

typedef enum
{
   LOC_ENGINE_STATE_ON  = 1,
   LOC_ENGINE_STATE_OFF = 2,
   LOC_ENGINE_STATE_MAX = 0x10000000 /* used to force enum to 32-bit */
} loc_engine_state_e_type;

typedef enum
{
   LOC_FIX_SESSION_STATE_BEGIN  = 1,
   LOC_FIX_SESSION_STATE_END    = 2,
   LOC_FIX_SESSION_STATE_MAX = 0x10000000 /* used to force enum to 32-bit */
} loc_fix_session_state_e_type;

typedef union
{
   loc_engine_state_e_type    engine_state;
   loc_fix_session_state_e_type   fix_session_state;
} loc_status_event_payload_u_type;
/*~ CASE LOC_STATUS_EVENT_ENGINE_STATE loc_status_event_payload_u_type.engine_state */
/*~ CASE LOC_STATUS_EVENT_FIX_SESSION_STATE loc_status_event_payload_u_type.fix_session_state */
/*~ DEFAULT loc_status_event_payload_u_type.void */

typedef struct
{
   loc_status_event_e_type                event;
   loc_status_event_payload_u_type        payload;
} loc_status_event_s_type;
/*~ FIELD loc_status_event_s_type.payload DISC loc_status_event_s_type.event */

/******************************************************************************
 *  Location server address types
 *****************************************************************************/
typedef enum
{
   LOC_SERVER_ADDR_IPV4 = 1,
   LOC_SERVER_ADDR_URL  = 2,
   LOC_SERVER_ADDR_IPV6 = 3,
   LOC_SERVER_ADDR_MAX = 0x10000000 /* used to force enum to 32-bit */
} loc_server_addr_e_type;

typedef struct
{
   uint32   addr;
   uint16   port;
} loc_server_addr_ipv4_type;

typedef struct
{
   uint16 length; /* Maximum will be LOC_API_MAX_SERVER_ADDR_LENGTH bytes*/
   char   addr[LOC_API_MAX_SERVER_ADDR_LENGTH];
} loc_server_addr_url_type; /* Server URL address */
/*~ FIELD loc_server_addr_url_type.addr
    VARRAY (loc_server_addr_url_type.length + 1)
 */

#define LOC_API_MAX_IPV6_ADDR_LENGTH 8
typedef struct {
  uint16  addr[LOC_API_MAX_IPV6_ADDR_LENGTH]; /* IPV6 server address */
  uint32  port;    /* IPV6 port id */
} loc_server_addr_ipv6_type;  /* IP V6 server address */

typedef union
{
   loc_server_addr_ipv4_type   ipv4;
   loc_server_addr_url_type    url;
   loc_server_addr_ipv6_type   ipv6;
} loc_server_addr_u_type;
/*~ CASE LOC_SERVER_ADDR_IPV4 loc_server_addr_u_type.ipv4 */
/*~ CASE LOC_SERVER_ADDR_URL loc_server_addr_u_type.url */
/*~ CASE LOC_SERVER_ADDR_IPV6 loc_server_addr_u_type.ipv6 */
/*~ DEFAULT loc_server_addr_u_type.void */

typedef struct
{
   loc_server_addr_e_type  addr_type;
   loc_server_addr_u_type  addr_info;
} loc_server_info_s_type;
/*~ FIELD loc_server_info_s_type.addr_info DISC loc_server_info_s_type.addr_type */

/******************************************************************************
 *  For LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST
 *****************************************************************************/
#define LOC_NI_MAX_REQUESTOR_ID_LENGTH          200
#define LOC_NI_SUPL_HASH_LENGTH                 8
#define LOC_NI_SUPL_SLP_SESSION_ID_BYTE_LENGTH  4
#define LOC_NI_MAX_CLIENT_NAME_LENGTH           64
#define LOC_NI_MAX_EXT_CLIENT_ADDRESS           20
#define LOC_NI_CODEWORD_LENGTH                  20

/* flags for notification */
#define  LOC_NI_CLIENT_NAME_PRESENT             0x0001
#define  LOC_NI_CLIENT_EXTADDR_PRESENT          0x0002
#define  LOC_NI_DEF_LOCATION_TYPE_PRESENT       0x0010
#define  LOC_NI_REQUESTOR_ID_PRESENT            0x0020
#define  LOC_NI_CODEWORD_PRESENT                0x0040
#define  LOC_NI_SERVICE_TYPE_ID_PRESENT         0x0080
#define  LOC_NI_ENCODING_TYPE_PRESENT           0x0100

typedef enum
{
   LOC_NI_USER_NO_NOTIFY_NO_VERIFY             = 1,
   LOC_NI_USER_NOTIFY_ONLY                     = 2,
   LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP     = 3,
   LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP = 4,
   LOC_NI_USER_PRIVACY_OVERRIDE                = 5,
   LOC_NI_USER_NOTIFY_VERITY_TYPE_MAX = 0x10000000 /* used to force enum to 32-bit */
} loc_ni_notify_verify_e_type;

typedef enum
{
   LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ       = 1,
   LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ     = 2,
   LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ  = 3,
   LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ = 4,
   LOC_NI_EVENT_MAX = 0x10000000 /* used to force enum to 32-bit */
} loc_ni_event_e_type;

typedef enum
{
   LOC_NI_PRESUPL_ISO646IRV = 0,
   LOC_NI_PRESUPL_ISO8859   = 1,
   LOC_NI_PRESUPL_UTF8      = 2,
   LOC_NI_PRESUPL_UTF16     = 3,
   LOC_NI_PRESUPL_UCS2      = 4,
   LOC_NI_PRESUPL_GSM_DEFAULT = 5,
   LOC_NI_PRESUPL_SHIFT_JIS   = 6,
   LOC_NI_PRESUPL_JIS =  7,
   LOC_NI_PRESUPL_EUC =  8,
   LOC_NI_PRESUPL_GB2312 = 9,
   LOC_NI_PRESUPL_CNS11643 = 10,
   LOC_NI_PRESUPL_KSC1001 = 11,
   LOC_NI_PRESUPL_ENCODING_UNKNOWN = 2147483647,

   LOC_NI_SS_GERMAN = 12,
   LOC_NI_SS_ENGLISH = 13,
   LOC_NI_SS_ITALIAN = 14,
   LOC_NI_SS_FRENCH = 15,
   LOC_NI_SS_SPANISH = 16,
   LOC_NI_SS_DUTCH = 17,
   LOC_NI_SS_SWEDISH = 18,
   LOC_NI_SS_DANISH = 19,
   LOC_NI_SS_PORTUGUESE = 20,
   LOC_NI_SS_FINNISH = 21,
   LOC_NI_SS_NORWEGIAN = 22,
   LOC_NI_SS_GREEK = 23,
   LOC_NI_SS_TURKISH = 24,
   LOC_NI_SS_HUNGARIAN = 25,
   LOC_NI_SS_POLISH = 26,
   LOC_NI_SS_LANGUAGE_UNSPEC = 27,

   LOC_NI_SUPL_UTF8 = 28,
   LOC_NI_SUPL_UCS2 = 29,
   LOC_NI_SUPL_GSM_DEFAULT = 30,
   LOC_NI_SUPL_ENCODING_UNKNOWN = 2147483647,
} loc_ni_datacoding_scheme_e_type; // Enum 32-bit

/*================= loc_vx_notify_verify_req_s ==================*/

typedef enum
{
   LOC_NI_VX_OCTET            = 0,
   LOC_NI_VX_EXN_PROTOCOL_MSG = 1,
   LOC_NI_VX_ASCII            = 2,
   LOC_NI_VX_IA5              = 3,
   LOC_NI_VX_UNICODE          = 4,
   LOC_NI_VX_SHIFT_JIS        = 5,
   LOC_NI_VX_KOREAN           = 6,
   LOC_NI_VX_LATIN_HEBREW     = 7,
   LOC_NI_VX_LATIN            = 8,
   LOC_NI_VX_GSM              = 9,
   LOC_NI_VX_ENCODING_TYPE_MAX = 0x10000000, /* used to force enum to 32-bit */
} loc_ni_vx_requester_id_encoding_scheme_e_type;

typedef enum
{
   LOC_VX_MS_ASSISTED_ONLY = 1,
   LOC_VX_MS_BASED_ONLY    = 2,
   LOC_VX_MS_ASSISTED_PREF_MSBASED_ALLWD = 3,
   LOC_VX_MS_BASED_PREF_ASSISTED_ALLWD   = 4,
   LOC_VX_POS_MODE_MAX = 0x10000000, /* used to force enum to 32-bit */
} loc_ni_vx_pos_mode_e_type;

typedef struct
{
   unsigned char requester_id_length;
   unsigned char requester_id[LOC_NI_MAX_REQUESTOR_ID_LENGTH];
} loc_ni_vx_requester_id_s_type;
/*~ FIELD loc_ni_vx_requester_id_s_type.requester_id
    VARRAY (loc_ni_vx_requester_id_s_type.requester_id_length + 1) */

typedef struct
{
   loc_ni_notify_verify_e_type                   notification_priv_type;
   unsigned char                                 pos_qos_incl;
   unsigned char                                 pos_qos;
   uint32                                        num_fixes;
   uint32                                        tbf;
   loc_ni_vx_pos_mode_e_type                     pos_mode;
   /* Encoding scheme applies for requester_id */
   loc_ni_vx_requester_id_encoding_scheme_e_type encoding_scheme;
   loc_ni_vx_requester_id_s_type                 requester_id;
   uint16                                        user_resp_timer_val; // in seconds
} loc_ni_vx_notify_verify_req_s_type;

/*================= loc_supl_notify_verify_req_s ==================*/

/* this maps supl_PosMethod enum. */
typedef enum
{
   LOC_NI_POSMETHOD_AGPS_SETASSISTED = 1,
   LOC_NI_POSMETHOD_AGPS_SETBASED = 2,
   LOC_NI_POSMETHOD_AGPS_SETASSISTED_PREF = 3,
   LOC_NI_POSMETHOD_AGPS_SETBASED_PREF = 4,
   LOC_NI_POSMETHOD_AUTONOMOUS_GPS = 5,
   LOC_NI_POSMETHOD_AFLT = 6,
   LOC_NI_POSMETHOD_ECID = 7,
   LOC_NI_POSMETHOD_EOTD = 8,
   LOC_NI_POSMETHOD_OTDOA = 9,
   LOC_NI_POSMETHOD_NO_POSITION = 10,
   LOC_NI_POSMETHOD_MAX = 0x10000000, /* used to force enum to 32-bit */
} loc_ni_supl_pos_method_e_type;


/* This list of defines applies for loc_ni_supl_notify_verify_req_s_type only.
   It is being used for the field of data_coding_scheme fields in struct loc_ni_requestor_id_s_type
   and loc_ni_supl_client_name_s_type in.  */
#define  LOC_NI_SUPL_FORMAT_LOGICAL_NAME   0x0
#define  LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS  0x1
#define  LOC_NI_SUPL_FORMAT_MSISDN         0x2
#define  LOC_NI_SUPL_FORMAT_URL            0x3
#define  LOC_NI_SUPL_FORMAT_SIP_URL        0x4
#define  LOC_NI_SUPL_FORMAT_MIN            0x5
#define  LOC_NI_SUPL_FORMAT_MDN            0x6
#define  LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY 0x7
#define  LOC_NI_SUPL_FORMAT_OSS_UNKNOWN    2147483647

typedef struct
{
   unsigned char                    presence;
   /* No encoding scheme for session_id */
   unsigned char                    session_id [LOC_NI_SUPL_SLP_SESSION_ID_BYTE_LENGTH];
   loc_server_info_s_type           slp_address;
} loc_ni_supl_slp_session_id_s_type;

/* Data coding scheme */
typedef struct
{
   /* For SUPL NI, data_coding_scheme actually refers to requestor format.
    * The list of applicable formats is specified in constancts: LOC_NI_SUPL_FORMAT_XXX.
    *
    * For CP, data_coding_scheme refers to loc_ni_datacoding_scheme_e_type.
    */
   unsigned char        data_coding_scheme;
   unsigned char        requestor_id_string[LOC_NI_MAX_REQUESTOR_ID_LENGTH];
   unsigned char        string_len;
} loc_ni_requestor_id_s_type;
/*~ FIELD loc_ni_requestor_id_s_type.requestor_id_string
   VARRAY (loc_ni_requestor_id_s_type.string_len + 1) */

/* LCS client name */
typedef struct
{
   /* For SUPL NI, data_coding_scheme actually refers to client name_format.
    * The list of applicable formats is specified in constancts: LOC_NI_SUPL_FORMAT_XXX.
    */
   unsigned char         data_coding_scheme;
   unsigned char         client_name_string[LOC_NI_MAX_CLIENT_NAME_LENGTH];
   unsigned char         string_len;
} loc_ni_supl_client_name_s_type;
/*~ FIELD loc_ni_supl_client_name_s_type.client_name_string
   VARRAY (loc_ni_supl_client_name_s_type.string_len + 1)
 */

#define LOC_NI_SUPL_QOP_VALID         0x01
#define LOC_NI_SUPL_QOP_VERACC_VALID  0x02
#define LOC_NI_SUPL_QOP_MAXAGE_VALID  0x04
#define LOC_NI_SUPL_QOP_DELAY_VALID   0x08

typedef struct
{
   unsigned char      bit_mask;    /* bit type indicating which fields are valid in this struct */
   unsigned char      horacc;
   unsigned char      veracc;      /* optional; set in bit_mask supl_veracc_present if present */
   uint16             maxLocAge;   /* optional; set in bit_mask supl_maxLocAge_present if present */
   unsigned char      delay;       /* optional; set in bit_mask supl_delay_present if  present */
} loc_ni_supl_qop_s_type;

/* SUPL Notify verify structure */
typedef struct
{
   loc_ni_notify_verify_e_type          notification_priv_type; /* Notification Type */
   uint16                               flags;                  /* Indicates which of the following are present. */
   loc_ni_supl_slp_session_id_s_type    supl_slp_session_id;    /* SLP Session ID */
   unsigned char                        supl_hash[LOC_NI_SUPL_HASH_LENGTH]; /* HASH for SUPL_INIT */
   loc_ni_datacoding_scheme_e_type      datacoding_scheme;      /* datacoding_scheme applies for both requestor_id and client_name */
   loc_ni_supl_pos_method_e_type        pos_method;             /* The GPS Mode to be used for the fix. */
   /* The encoding scheme for requestor_id is specified in data_coding_scheme.
      The data_coding_scheme inside of requestor_id is used to specify requestor id format */
   loc_ni_requestor_id_s_type           requestor_id;           /* Requestor ID */
   /* The encoding scheme for client_name is specified in data_coding_scheme.
      The data_coding_scheme inside of client_name is used to specify client_name format */
   loc_ni_supl_client_name_s_type       client_name;            /* Client Name */
   loc_ni_supl_qop_s_type               supl_qop;               /* supl qop */
   uint16                               user_response_timer;    /* Time before which a user has to respond. Not used currently */
   /* supl_ver2_ext_valid must be set to true if SUPL ver 2 extension
   payload is being passed */
   boolean                              supl_ver2_ext_valid;    
   /* SUPL ver 2 extension payload */
   qmiLocNiSuplVer2ExtStructT_v02       supl_ver2_ext;   
   
   /* emergency services validity bit*/
   boolean                              supl_emergency_valid;
   /* emergency services URL */
   qmiLocEmergencyNotificationStructT_v02  supl_emergency_notification;
          
} loc_ni_supl_notify_verify_req_s_type;

/*================= loc_umts_cp_notify_verify_req_s ==================*/

typedef struct
{
   unsigned char    ext_client_address_len; /* External Client Address Length */
   unsigned char    ext_client_address[LOC_NI_MAX_EXT_CLIENT_ADDRESS]; /* This field is valid only the flags
                                                                      indicate ext client id pres. Supported only for CP */
   /*~ FIELD loc_ni_ext_client_address_s_type.ext_client_address
      VARRAY (loc_ni_ext_client_address_s_type.ext_client_address_len + 1)
    */
} loc_ni_ext_client_address_s_type;

typedef enum
{
   LOC_NI_LOCATIONTYPE_CURRENT_LOCATION = 1,
   LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION = 2,
   LOC_NI_LOCATIONTYPE_INITIAL_LOCATION = 3,
   LOC_NI_LOCATIONTYPE_MAX = 0x10000000, /* used to force enum to 32-bit */
} loc_ni_location_type_e_type;

/* Defererred location */
typedef struct
{
   unsigned char    unused_bits;
   unsigned char    ms_available;
} loc_ni_deferred_location_s_type;

/* LCS code word */
typedef struct
{
   unsigned char       data_coding_scheme;
   unsigned char       lcs_codeword_string[LOC_NI_CODEWORD_LENGTH];
   unsigned char       string_len;
} loc_ni_codeword_string_s_type;
/*~ FIELD loc_ni_codeword_string_s_type.lcs_codeword_string
   VARRAY (loc_ni_codeword_string_s_type.string_len + 1)
 */

/* LCS Service ID */
typedef struct
{
   unsigned char       lcs_service_type_id;
} loc_ni_service_type_id_s_type;

/* UMTS CP notify verify structure */
typedef struct
{
   loc_ni_notify_verify_e_type           notification_priv_type; /* Notification Type */
   unsigned char                         invoke_id;              /* SS Invoke Id  */
   uint16                                flags;                  /* Indicates which of the following are present */
   unsigned char                         notification_length;
   /* Notification Text valid only if the flags indicate notification text present */
   char                                  notification_text[LOC_NI_MAX_CLIENT_NAME_LENGTH] /* Encoding method is specified in datacoding_scheme */;
   loc_ni_datacoding_scheme_e_type       datacoding_scheme;      /* Type of Data encoding scheme for the text.
                                                                    Applies for both notification_text and ext_client_address_data */
   loc_ni_ext_client_address_s_type      ext_client_address_data;/* Encoding method is specified in datacoding scheme */
   loc_ni_location_type_e_type           location_type;          /* Location Type sent only for CP */
   loc_ni_deferred_location_s_type       deferred_location;      /* Deferred Location - CP Only */
   loc_ni_requestor_id_s_type            requestor_id;           /* Requestor ID, encoding method is specified in the field of data_coding_scheme
                                                                    in requestor_id struct and the list of encoding method is defined in
                                                                    loc_ni_datacoding_scheme_e_type */
   loc_ni_codeword_string_s_type         codeword_string;        /* Codeword String - CP Only. encoding method is specified in field of
                                                                  * data_coding_scheme in codeword_string struct and the list of encoding method
                                                                  * is defined in loc_ni_datacoding_scheme_e_type
                                                                  */
   loc_ni_service_type_id_s_type         service_type_id;        /* Service Type ID - CP Only */
   uint16                                user_response_timer;    /* Time before which a user has to respond. Not used currently */
} loc_ni_umts_cp_notify_verify_req_s_type;
/*~ FIELD  loc_ni_umts_cp_notify_verify_req_s_type.notification_text
    VARRAY (loc_ni_umts_cp_notify_verify_req_s_type.notification_length + 1)
 */

typedef enum
{  
  LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO = 1,  /* Service interaction between ongoing-ni session and incoming MO session */
  LOC_NI_SERVICE_INTERACTION_MAX = 0x10000000            /* used to force enum to 32-bit */
} loc_ni_service_interaction_e_type;

typedef struct
{
   loc_ni_vx_notify_verify_req_s_type ni_vx_req;              /* On-going NI session req, info not currently filled */
   loc_ni_service_interaction_e_type  service_interation_type;
}loc_ni_vx_service_interaction_req_s_type;

/*================= loc_ni_event_payload_u ==================*/

typedef union
{
   loc_ni_vx_notify_verify_req_s_type        vx_req;
   loc_ni_supl_notify_verify_req_s_type      supl_req;                /* SUPL request */
   loc_ni_umts_cp_notify_verify_req_s_type   umts_cp_req;             /* UMTS CP request */
   loc_ni_vx_service_interaction_req_s_type  service_interaction_req; /* Service interaction request*/
} loc_ni_event_payload_u_type;
/*~ CASE LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ        loc_ni_event_payload_u_type.vx_req */
/*~ CASE LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ      loc_ni_event_payload_u_type.supl_req */
/*~ CASE LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ   loc_ni_event_payload_u_type.umts_cp_req */
/*~ CASE LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ  loc_ni_event_payload_u_type.service_interaction_req */
/*~ DEFAULT                                       loc_ni_event_payload_u_type.void */

/*================= loc_ni_event_s ==================*/

typedef struct
{
   loc_ni_event_e_type            event;
   loc_ni_event_payload_u_type    payload;
} loc_ni_event_s_type;
/*~ FIELD loc_ni_event_s_type.payload DISC loc_ni_event_s_type.event */


/******************************************************************************
 * For LOC_EVENT_ASSISTANCE_DATA_REQUEST
 * This event will used for location engine to request assistance data from app.
 *****************************************************************************/
typedef enum
{
   LOC_ASSIST_DATA_TIME_REQ = 1,             /* Engine requests time from user */
   LOC_ASSIST_DATA_PREDICTED_ORBITS_REQ = 2, /* Engine requests xtra from user */
   LOC_ASSIST_DATA_POSITION_INJECTION_REQ = 3, /* Engine requests position injection from user */
   LOC_ASSIST_DATA_MAX = 0x10000000,
} loc_assist_data_request_e_type;

typedef struct
{
   uint32   delay_threshold;   /* recommended to skip a server  if
                                  one-way delay exceeds this threshold */
   char*    servers[LOC_API_MAX_NUM_NTP_SERVERS];
} loc_time_download_source_s_type;
/*~ FIELD loc_time_download_source_s_type.servers
    loc_time_download_source_s_type.servers[] STRING LOC_API_MAX_SERVER_ADDR_LENGTH
 */

typedef struct
{
   // Maximum size of file (for all parts) in bytes.
   uint32                                   max_file_size;
   // Maximum part size in bytes.
   uint32                                   max_part_size;
   char*                                    servers[LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS];
} loc_predicted_orbits_data_source_s_type;
/*~ FIELD loc_predicted_orbits_data_source_s_type.servers
    loc_predicted_orbits_data_source_s_type.servers[] STRING LOC_API_MAX_SERVER_ADDR_LENGTH
 */

#define LOC_POS_INJ_REQ_LATITUDE_VALID  0x01
#define LOC_POS_INJ_REQ_LONGITUDE_VALID 0x02
#define LOC_POS_INJ_REQ_PUNC_VALID      0x04
#define LOC_POS_INJ_REQ_TIMESTAMP_VALID 0x08

typedef struct {
   uint32 flags;
   double latitude;
   double longitude;
   uint32 position_uncertainty;
   uint64 timestamp;
} loc_pos_inj_request_s_type;

typedef union
{
   loc_time_download_source_s_type            time_download;
   loc_predicted_orbits_data_source_s_type    data_download;
   loc_pos_inj_request_s_type                 pos_injection;
} loc_assist_data_request_payload_u_type;
/*~ CASE LOC_ASSIST_DATA_TIME_REQ loc_assist_data_request_payload_u_type.time_download */
/*~ CASE LOC_ASSIST_DATA_PREDICTED_ORBITS_REQ loc_assist_data_request_payload_u_type.data_download */
/*~ CASE LOC_ASSIST_DATA_POSITION_INJECTION_REQ loc_assist_data_request_payload_u_type.pos_injection */
/*~ DEFAULT loc_assist_data_request_payload_u_type.void */

typedef struct
{
   loc_assist_data_request_e_type           event;
   loc_assist_data_request_payload_u_type   payload;
} loc_assist_data_request_s_type;
/*~ FIELD loc_assist_data_request_s_type.payload DISC loc_assist_data_request_s_type.event */

/******************************************************************************
 * For LOC_EVENT_LOCATION_SERVER_REQUEST
 * NOTE: structure in this section will be revised in subsequent revisions
 *****************************************************************************/
typedef uint32 loc_server_connection_handle;

/* Protocol deployed on location server */
typedef enum
{
  LOC_SERVER_PROTOCOL_DEFAULT = 0,   /* In Loc API 1.0, A-proc will only do open/close connection,
                                        so, there is no need to specify the protocol type */
  LOC_SERVER_PROTOCOL_SUPL    = 1,   /* SUPL Location server */
  LOC_SERVER_PROTOCOL_VX_MPC  = 2,   /* VX MPC server for untrusted apps */
  LOC_SERVER_PROTOCOL_VX_PDE  = 3,   /* VX PDE server for trusted apps */
  LOC_SERVER_PROTOCOL_MAX = 0x1000000
} loc_server_protocol_e_type;

/* Connection type */
typedef enum
{
  LOC_SERVER_CONNECTION_LBS = 0,       /*LBS connection*/   
  LOC_SERVER_CONNECTION_WWAN_INTERNET,  /*ANY WWAN Internet connection*/
  LOC_SERVER_CONNECTION_MAX = 0x1000000 /* force enum to 32 bits*/
} loc_server_connection_e_type;

/* Request type */
typedef enum
{
   LOC_SERVER_REQUEST_OPEN  = 1, /* Connection to be opened */
   LOC_SERVER_REQUEST_CLOSE = 2, /* Connection to be closed */   
   LOC_SERVER_REQUEST_MULTI_OPEN  = 3, /* Connection to be opened */
   LOC_SERVER_REQUEST_MAX = 0x10000000, /* force enum to 32 bits*/
} loc_server_request_e_type;

/* Param when opening the connection: data structure is pending future enhancement */
typedef struct
{
   loc_server_connection_handle  conn_handle;
   loc_server_protocol_e_type    protocol;
} loc_server_open_req_s_type;

/* Param when opening multiple connection */
typedef struct
{
   loc_server_connection_handle  conn_handle;
   loc_server_protocol_e_type    protocol;
   qmiLocWWANTypeEnumT_v02  connection_type;
} loc_server_multi_open_req_s_type;

/* Param when closing the connection */
typedef struct
{
   loc_server_connection_handle  conn_handle;
} loc_server_close_req_s_type;

/* Payload */
typedef union
{
   loc_server_open_req_s_type  open_req;
   loc_server_close_req_s_type close_req;
   loc_server_multi_open_req_s_type  multi_open_req;   
} loc_server_request_u_type;
/*~ CASE LOC_SERVER_REQUEST_OPEN loc_server_request_u_type.open_req */
/*~ CASE LOC_SERVER_REQUEST_CLOSE loc_server_request_u_type.close_req */
/*~ CASE LOC_SERVER_REQUEST_MULTI_OPEN loc_server_request_u_type.multi_open_req */
/*~ DEFAULT loc_server_request_u_type.void */

typedef struct
{
   loc_server_request_e_type       event;
   loc_server_request_u_type       payload;
} loc_server_request_s_type;
/*~ FIELD loc_server_request_s_type.payload DISC loc_server_request_s_type.event */

/******************************************************************************
 * For LOC_EVENT_RESERVED
 *****************************************************************************/
/* Reserved for internal development */
typedef struct
{
   uint16   data_size;
   byte*    data;
} loc_reserved_payload_s_type;
/*~ FIELD loc_reserved_payload_s_type.data
    VARRAY LENGTH loc_reserved_payload_s_type.data_size */

/*=============================================================================
 *
 *           Location IOCTL API and callback data (by events)
 *
 *============================================================================*/

/*******************************************************************************
 * Location ioctl command enum
 *****************************************************************************/
typedef enum
{
   LOC_IOCTL_GET_API_VERSION                          = 1,
   LOC_IOCTL_SET_FIX_CRITERIA                         = 2,
   LOC_IOCTL_GET_FIX_CRITERIA                         = 3,

   LOC_IOCTL_SERVICE_START_INDEX                      = 400,
   LOC_IOCTL_INFORM_NI_USER_RESPONSE                  = 400,
   LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA             = 401,
   LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY     = 402,
   LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE       = 403,
   LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD  = 404,
   LOC_IOCTL_INJECT_UTC_TIME                          = 405,
   LOC_IOCTL_INJECT_RTC_VALUE                         = 406,
   LOC_IOCTL_INJECT_POSITION                          = 407,
   LOC_IOCTL_QUERY_ENGINE_STATE                       = 408,
   LOC_IOCTL_INFORM_SERVER_OPEN_STATUS                = 409,
   LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS               = 410,
   LOC_IOCTL_ACCESS_EFS_DATA                          = 413,
   LOC_IOCTL_ERROR_ESTIMATE_CONFIG                    = 414,   
   LOC_IOCTL_INFORM_SERVER_MULTI_OPEN_STATUS          = 415,

   LOC_IOCTL_NV_SETTINGS_START_INDEX                  = 800,
   LOC_IOCTL_SET_ENGINE_LOCK                          = 800,
   LOC_IOCTL_GET_ENGINE_LOCK                          = 801,
   LOC_IOCTL_SET_SBAS_CONFIG                          = 802,
   LOC_IOCTL_GET_SBAS_CONFIG                          = 803,
   LOC_IOCTL_SET_NMEA_TYPES                           = 804,
   LOC_IOCTL_GET_NMEA_TYPES                           = 805,

   LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR                 = 806,
   LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR                 = 807,
   LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR                 = 808,
   LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR                 = 809,
   LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR                 = 810,
   LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR                 = 811,

   LOC_IOCTL_SET_ON_DEMAND_LPM                        = 812,
   LOC_IOCTL_GET_ON_DEMAND_LPM                        = 813,

   // for XTRA-T support
   LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL               = 814,
   LOC_IOCTL_GET_XTRA_T_SESSION_CONTROL               = 815,

   //for APN Profiles 
   LOC_IOCTL_SET_LBS_APN_PROFILE                      = 816,
   LOC_IOCTL_GET_LBS_APN_PROFILE                      = 817,   
   LOC_IOCTL_SET_XTRA_APN_PROFILE                     = 818,
   LOC_IOCTL_GET_XTRA_APN_PROFILE                     = 819,

   //for enabling/disabling data
   LOC_IOCTL_SET_DATA_ENABLE                          = 820, 

   // for SUPL version
   LOC_IOCTL_SET_SUPL_VERSION                         = 821,
   LOC_IOCTL_GET_SUPL_VERSION = 822,

   LOC_IOCTL_PROPRIETARY_START_INDEX                  = 1000,
   LOC_IOCTL_DELETE_ASSIST_DATA                       = 1000,
   // Set the pde server address and port to be used only for this location client
   LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR               = 1001,
   LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR               = 1002, 
   LOC_IOCTL_SET_EVENT_REG_MASK                       = 1003,
   LOC_IOCTL_GET_EVENT_REG_MASK                       = 1004, 
   LOC_IOCTL_SET_OPERATION_MODE                       = 1005,
   LOC_IOCTL_GET_OPERATION_MODE                       = 1006,
   LOC_IOCTL_SET_SUPL_SECURITY                        = 1007,
   LOC_IOCTL_GET_SUPL_SECURITY                        = 1008,
   LOC_IOCTL_GET_VX_CONFIG                            = 1009,
   LOC_IOCTL_SET_VX_CONFIG                            = 1010,
   LOC_IOCTL_SET_PE_CONFIG                            = 1011,
   LOC_IOCTL_GET_PE_CONFIG                            = 1012,

   // LTE positioning protocol configuration
   LOC_IOCTL_SET_LPP_CONFIG                           = 1013,
   LOC_IOCTL_GET_LPP_CONFIG                           = 1014,

   //Best available position
   LOC_IOCTL_GET_BEST_AVAILABLE_POSITION              = 1015,
 
   //Assisted Glo configuration
   LOC_IOCTL_SET_ASSISTED_GLO_CONFIG                  = 1016,
   LOC_IOCTL_GET_ASSISTED_GLO_CONFIG                  = 1017, 
   
   // Inject GSM Cell Info
   LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ                 = 1018, 

   // Inject NI Message
   LOC_IOCTL_INJECT_NI_MESSAGE_REQ                    = 1019,

   //WWAN OOS notification
   LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ                = 1020,
 
   //Emergency Protocol
   LOC_IOCTL_SET_EMERGENCY_PROTOCOL                   = 1021,
   LOC_IOCTL_GET_EMERGENCY_PROTOCOL                   = 1022,  

   //SUPL Hash Algo
   LOC_IOCTL_SET_SUPL_HASH_ALGO                       = 1023,
   LOC_IOCTL_GET_SUPL_HASH_ALGO                       = 1024,

   //SUPL TLS version
   LOC_IOCTL_SET_SUPL_TLS_VERSION                     = 1025, 
   LOC_IOCTL_GET_SUPL_TLS_VERSION                     = 1026, 

   //WiFi scan injection timeout
   LOC_IOCTL_SET_WIFI_SCAN_INJECT_TIMEOUT             = 1027, 
   LOC_IOCTL_GET_WIFI_SCAN_INJECT_TIMEOUT             = 1028,

   // Available WWAN position
   LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION              = 1029,

   // Premium Services Config
   LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG              = 1030,

   //XTRA version check
   LOC_IOCTL_SET_XTRA_VERSION_CHECK                   = 1031,

   //GNSS Satellite constellation configuration
   LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG          = 1032, 

   // Available  position estimate
   LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC          = 1035,

   //Delete GNSS service data
   LOC_IOCTL_DELETE_GNSS_SERVICE_DATA                 = 1036,
   
   LOC_IOCTL_RESERVED_CMD                             = 8000,

   LOC_IOCTL_THIRD_PARTY_START_INDEX                  = 0x40000000,

} loc_ioctl_e_type;

/******************************************************************************
 * For LOC_IOCTL_GET_API_VERSION
 * The API version will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 *****************************************************************************/
typedef struct
{
   unsigned char major;
   unsigned char minor;
} loc_api_version_s_type;

/******************************************************************************
 * For LOC_IOCTL_GET(SET)_FIX_CRITERIA
 * The status will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 * When LOC_IOCTL_GET_PROVIDER_CRITERIA is issued and the ioctl_status indicates that
 * the call is successful, the fix criteria will be available in the event payload.
 *****************************************************************************/
#define LOC_FIX_CRIT_VALID_RECURRENCE_TYPE                  0x00000001
#define LOC_FIX_CRIT_VALID_PREFERRED_OPERATION_MODE         0x00000002
#define LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY               0x00000004
#define LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME          0x00000008
#define LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED  0x00000010
#define LOC_FIX_CRIT_VALID_NOTIFY_TYPE                      0x00000020
#define LOC_FIX_CRIT_VALID_MIN_INTERVAL                     0x00000040
#define LOC_FIX_CRIT_VALID_MIN_DISTANCE                     0x00000080
#define LOC_FIX_CRIT_VALID_MIN_DIST_SAMPLE_INTERVAL         0x00000100
#define LOC_FIX_CRIT_VALID_APPLICATION_ID                   0x00000200

typedef enum
{
   LOC_PERIODIC_FIX = 1,
   LOC_SINGLE_FIX   = 2,
   LOC_FIX_SESSION_TYPE_MAX = 0x10000000, /* Force enum to be 32-bit */
} loc_fix_recurrence_e_type;

/* Notification on distance may not be supported on phase 1 */
typedef enum
{
   /* Notify when min time interval has elapsed, distance do not care */
   LOC_NOTIFY_ON_INTERVAL = 1,
   /* Notify when min distance has moved, interval do not care */
   LOC_NOTIFY_ON_DISTANCE = 2,
   /* Notify when either min internal has elapsed or min distance has moved */
   LOC_NOTIFY_ON_ANY = 3,
   /* Notify only when both the min interval has elapsed and min distance has moved */
   LOC_NOTIFY_ON_ALL = 4,
   LOC_NOTIFY_TYPE_MAX = 0x10000000, /* Force enum to be 32 bit*/
}loc_notify_e_type;

typedef struct
{
   uint32                             valid_mask;
   loc_fix_recurrence_e_type          recurrence_type;          /* Default is periodic fixes */
   qmiLocOperationModeEnumT_v02       preferred_operation_mode; /* Mode used as guidance to the engine, actual mode may vary*/
   // preferred accuracy and response time are used to driver location method selection
   uint32                             preferred_accuracy;       /* in meters, default is 50 meters */
   uint32                             preferred_response_time;  /* in milli-seconds, default is 255 seconds */
   boolean                            intermediate_pos_report_enabled; /* Default is on, currently not supported in 1.0*/
   loc_notify_e_type                  notify_type;              /* Notify type based on distance is not supported in 1.0*/
   uint32                             min_interval;             /* in-milli-seconds */
   float                              min_distance;             /* in meters */
   uint32                             min_dist_sample_interval; /* in milli-seconds only applies when notifiation based on min_distance is used */
   qmiLocApplicationIdStructT_v02     application_id;           /* application ID */
   uint32                             min_intermediate_interval;/* in milli-seconds */
} loc_fix_criteria_s_type;

/******************************************************************************
 * For LOC_IOCTL_INFORM_NI_USER_RESPONSE
 * Processing status will be sent back to the client
 * via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 *****************************************************************************/

/* Notify verify response type */
typedef enum
{
   LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT = 1,
   LOC_NI_LCS_NOTIFY_VERIFY_DENY   = 2,
   LOC_NI_LCS_NOTIFY_VERIFY_NORESP = 3,
   LOC_NI_LCS_NOTIFY_VERIFY_MAX = 0x10000000, /* force enum to be 32-bit */
} loc_ni_user_resp_e_type;

typedef struct
{
   loc_ni_user_resp_e_type   user_resp;
   loc_ni_event_s_type       ni_event_pass_back;
} loc_user_verify_s_type;

/******************************************************************************
 * For LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA
 * Client calls IOCTL in a row to inject all parts of predicted orbits data into
 * the location engine. The processing status will be sent back to the client
 * via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 * Pseudo code will be like:
 *
 * for ( all parts of data)
 *  loc_ioctl (LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA):
 * wait for status callback
 *****************************************************************************/

typedef enum
{
   LOC_PREDICTED_ORBITS_XTRA = 0,  /* Default is QCOM-XTRA format */
   LOC_PREDICTED_ORBITS_FORMAT_MAX = 0x10000000, /* force enum to be 32-bit */
} loc_predicted_orbits_data_format_e_type;

typedef struct
{
   loc_predicted_orbits_data_format_e_type format_type;
   uint32              total_size;   /* total size for this predicted orbits file */
   uint8               total_parts;  /* total part for this xtra file */
   uint16               part;         /* part number for this xtra data; start with 1 */
   uint16              part_len;     /* length of xtra data for this part */
   unsigned char*      data_ptr;     /* xtra data pointer */
}loc_predicted_orbits_data_s_type;
/*~ FIELD loc_predicted_orbits_data_s_type.data_ptr
    VARRAY LENGTH loc_predicted_orbits_data_s_type.part_len */

/******************************************************************************
 * For LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE
 * This IOCTL is used to query the server address that hosts the predicted orbits data
 * and its file size and etc. IOCTL status will be sent back to the calling Location
 * client via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 * If the the query is successful, loc_predicted_orbits_data_source_s_type will
 * be included in the callback.
 *****************************************************************************/
/* Data structure used for this event is loc_predicted_orbits_data_source_s_type */

/******************************************************************************
 * For LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY
 * This IOCTL is used to query predicted orbits data validity. IOCTL status will
 * be sent back to the calling Location client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT. When the query is successful,
 * loc_predicted_orbits_data_validity_report_s_type will be included in the callback
 *****************************************************************************/
typedef struct
{
  uint64   start_time_utc; /* Seconds in UTC time of 1970, Jan 1st: this predicted orbits is valid starting this time*/
  uint16   valid_duration_hrs;
} loc_predicted_orbits_data_validity_report_s_type;

/******************************************************************************
 * For LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD
 * Processing status will be sent back to the client
 * via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 *****************************************************************************/
typedef struct
{
   boolean           enable;
   unsigned char     auto_check_every_hrs;           // In hours
} loc_predicted_orbits_auto_download_config_s_type;

/******************************************************************************
 * For LOC_IOCTL_INJECT_UTC_TIME
 * Processing status will be sent back to the client
 * via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 *****************************************************************************/

typedef struct
{
   uint64  time_utc;         /* In milli-seconds, UTC time since Jan 1st, 1970 */
   uint32  uncertainty;      /* In milli-secodns, assoc. time uncertainty */
} loc_assist_data_time_s_type;


/******************************************************************************
 * For LOC_IOCTL_INJECT_RTC_VALUE
 * Data structure yet to be defined
 *****************************************************************************/

/******************************************************************************
 * For LOC_IOCTL_INJECT_POSITION.
 * Processing status will be sent back to the client
 * via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 *****************************************************************************/

typedef uint64 loc_assist_pos_valid_mask_type;

#define LOC_ASSIST_POS_VALID_TIMESTAMP_UTC                0x00000001
#define LOC_ASSIST_POS_VALID_LATITUDE                     0x00000002
#define LOC_ASSIST_POS_VALID_LONGITUDE                    0x00000004
#define LOC_ASSIST_POS_VALID_ALTITUDE_WRT_ELLIPSOID       0x00000008
#define LOC_ASSIST_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL  0x00000010
#define LOC_ASSIST_POS_VALID_HOR_UNC_CIRCULAR             0x00000020
#define LOC_ASSIST_POS_VALID_VERT_UNC                     0x00000040
#define LOC_ASSIST_POS_VALID_CONFIDENCE_HORIZONTAL        0x00000080
#define LOC_ASSIST_POS_VALID_CONFIDENCE_VERTICAL          0x00000100
#define LOC_ASSIST_POS_VALID_TIMESTAMP_AGE                0x00000200


#define LOC_ASSIST_POS_VALID_H_RELIABILITY_IND            0x00010000
#define LOC_ASSIST_POS_VALID_V_RELIABILITY_IND            0x00020000
#define LOC_ASSIST_POS_VALID_ALTITUDE_SOURCE              0x00040000
#define LOC_ASSIST_POS_VALID_ALT_H_LINKAGE                0x00080000
#define LOC_ASSIST_POS_VALID_ALT_UNC_COVERAGE             0x00100000
#define LOC_ASSIST_POS_VALID_POSITION_SRC                 0x00200000
#define LOC_ASSIST_POS_VALID_RAW_HOR_UNC_CIRCULAR         0x00400000
#define LOC_ASSIST_POS_VALID_RAW_CONFIDENCE_HORIZONTAL    0x00800000


typedef struct
{
   /* indicating the presence/absence of the optional fields */
   loc_assist_pos_valid_mask_type           valid_mask;

   /* info. of the time associated with the position. */
   uint64           timestamp_utc;           /* in milli-seconds, utc time, Jan 1, 1970*/

   double           latitude;             /* Degrees latitude.  North is positive */
   double           longitude;            /* Degrees longitude.  East is positive */

   float            altitude_wrt_ellipsoid;   /* alt: meters, positive height, negative depth  */

   float            altitude_wrt_mean_sea_level;

   /* horizontal unc: circular shape, radial, meters
      This field is mandatory if both latitude and longitude fields are specified */
   float            hor_unc_circular;

   /* vertical unc, in meters
      This field is mandatory if either altitude_wrt_ellipsoid or altitude_wrt_mean_sea_level is specified */
   float            vert_unc;        /* loc unc vertical: meters */

   /* Confidence horizontal: percentage, as defined by  ETSI TS 101 109.
     0 - no info, 100-255 not used, will be rejected; 
     if 100 is received, re-interpret to 99.
     This field need to be specified in pair with the field of hor_unc_circular.
     If not specified when hor_unc_circular is set, the default value will be 50.  */
   uint8            confidence_horizontal;

   /* Confidence vertical: percentage, as defined by  ETSI TS 101 109.
     0 - no info. to be rejected; 100-256 not used; if 100 is received, re-interpret to 99.
     This field need to be specified in pair with the field of vert_unc.
     If not specified, the default value will be 50.  */
   unsigned char                           confidence_vertical;

   /* Relative position age in milliseconds */
   int32            timestamp_age;

  pdsm_pd_reliability_e_type hor_reliability; /*horizontal reliability*/
  pdsm_pd_reliability_e_type vert_reliability; /*vertical reliability*/
  
  /* source of altitude in injected position */
  pdsm_pd_altitude_source_e_type             altitude_source;
  /* linkage / dependence between injected horizontal position and altitude */
  pdsm_pd_altitude_horizontal_linkage_e_type altitude_hor_linkage;
  /* uncertainty coverage of the injected altitude */
  pdsm_pd_altitude_unc_coverage_e_type       altitude_unc_coverage;
  /* Position source of the injected position */
  pdsm_pd_extern_coarse_pos_source_e_type    position_source;

   /* raw horizontal unc: circular shape, radial, meters */
   float              raw_hor_unc_circular;

   /* raw confidence horizontal: percentage, as defined by  ETSI TS 101 109.
     0 - no info, 100-255 not used, will be rejected; 
     if 100 is received, re-interpret to 99.
     This field need to be specified in pair with the field of raw_hor_unc_circular.
     If not specified when raw_hor_unc_circular is set, the default value will be 50.  */
   uint8              raw_confidence_horizontal;

} loc_assist_data_pos_s_type;

/******************************************************************************
 * For LOC_IOCTL_QUERY_ENGINE_STATE.
 * This IOCTL is used to query the location engine state.
 *****************************************************************************/
/* Data structure yet to be defined */

/******************************************************************************
 * For LOC_IOCTL_INFORM_SERVER_OPEN_STATUS
 * This IOCTL is used to inform location engine regarding the processing status
 * to open data connection. The request to open data connetion is sent via
 * the callback registered through loc_open with the event set to
 * LOC_EVENT_LOCATION_SERVER_REQUEST.
 *****************************************************************************/
#define  LOC_MAX_APN_STRING_LEN    100   /* Max length of APN string    */

typedef enum
{
   LOC_SERVER_OPEN_SUCCESS = 1,
   LOC_SERVER_OPEN_FAIL = 2,
   LOC_SERVER_OPEN_STATUS_MAX = 0x10000000,   /* Force enum to be 32 bit */
}loc_server_open_status_e_type;

typedef enum
{
  LOC_SERVER_PDP_IP=0x0,                     /* PDP type IP (IPV4)       */
  LOC_SERVER_PDP_PPP,                        /* PDP type PPP             */
  LOC_SERVER_PDP_IPV6,                       /* PDP type IPV6            */
  LOC_SERVER_PDP_IPV4V6,                     /* PDP type IPV4V6          */
  LOC_SERVER_PDP_MAX=0x10000000              /* Force enum to be 32 bits */
} loc_server_pdp_type_e_type;

typedef struct
{
   loc_server_connection_handle              conn_handle;
   loc_server_open_status_e_type             open_status;
   // when open successful for UMTS, need to pass the apn name
   char                                      apn_name[LOC_MAX_APN_STRING_LEN];
}loc_server_open_status_s_type;

typedef struct
{
   loc_server_connection_handle              conn_handle;
   loc_server_open_status_e_type             open_status;
   // when open successful for UMTS, need to pass the apn name and pdp type
   loc_server_pdp_type_e_type                pdp_type;
   char                                      apn_name[LOC_MAX_APN_STRING_LEN];
}loc_server_multi_open_status_s_type;


/******************************************************************************
 * For LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS
 * This IOCTL is used to inform location engine regarding the processing status
 * to close data connection. The request to close data connetion is sent via
 * the callback registered through loc_open with the event set to
 * LOC_EVENT_LOCATION_SERVER_REQUEST.
 *****************************************************************************/

typedef enum
{
   LOC_SERVER_CLOSE_SUCCESS    = 1,
   LOC_SERVER_CLOSE_FAIL       = 2,
   LOC_SERVER_CLOSE_STATUS_MAX = 0x10000000, /* Force enum to be 32-bit */
}loc_server_close_status_e_type;

typedef struct
{
  loc_server_connection_handle          conn_handle;
  loc_server_close_status_e_type        close_status;
}loc_server_close_status_s_type;

/******************************************************************************
 * For LOC_IOCTL_ACCESS_EFS_DATA
 *****************************************************************************/

/*  GPS FS task operations */
typedef enum {
   LOC_FS_CREATE_WRITE_FILE = 1,
   LOC_FS_APPEND_FILE       = 2,
   LOC_FS_DELETE_FILE       = 3,
   LOC_FS_READ_FILE         = 4,
   /* Delete all regular (not links, dirs etc.) files in a dir */
   LOC_FS_PURGE_DIR         = 5,
   LOC_FS_MAX               = 0x10000000 /* Force enum to be 32-bit */
} loc_fs_operation_e_type;

#define LOC_EFS_MAX_PATH_LEN_BYTES  64     /* Max file name length in bytes that can be written*/
#define LOC_EFS_MAX_FILE_LEN_BYTES  2000   /* Max file size in bytes that can be written */

typedef struct {
   char                 filename[LOC_EFS_MAX_PATH_LEN_BYTES]; /* NULL terminated Filename array . */
   loc_fs_operation_e_type  operation;           /* File Operation. Only LOC_FS_CREATE_WRITE_FILE,
                                                    LOC_FS_DELETE_FILE are supported */
   uint32               total_size;              /* Total length (in bytes) of the file, max length defined by LOC_EFS_MAX_FILE_LEN_BYTES */
   unsigned char       *data_ptr;                /* Data Content */
   uint32               part_len;                /* Length of a part, limited by RPC */
   uint8                part;                    /* data part number, starting with 1 */
   uint8                total_parts;             /* total data parts */
   uint32               reserved;                /* Reserved for future use */
} loc_efs_data_s_type;
/*~ FIELD loc_efs_data_s_type.data_ptr VARRAY LENGTH loc_efs_data_s_type.part_len */

/******************************************************************************
 * For LOC_IOCTL_ERROR_ESTIMATE_CONFIG
 *****************************************************************************/
typedef enum
{
   LOC_ERROR_ESTIMATE_CONFIG_SET   = 1,
   LOC_ERROR_ESTIMATE_CONFIG_CLEAR = 2,
   LOC_ERROR_ESTIMATE_MAX          = 0x10000000
} loc_error_estimate_config_e_type;

/******************************************************************************
 * For LOC_IOCT_SET(GET)_LBS(XTRA)_APN_PROFILE
 *****************************************************************************/
#define LOC_MAX_APN_NAME_LENGTH 100
#define LOC_MAX_APN_PROFILES    6

/*APN Profile Serving Systems*/
#define LOC_APN_PROFILE_SRV_SYS_CDMA  0x01
#define LOC_APN_PROFILE_SRV_SYS_HDR   0x02
#define LOC_APN_PROFILE_SRV_SYS_GSM   0x04
#define LOC_APN_PROFILE_SRV_SYS_WCDMA 0x08
#define LOC_APN_PROFILE_SRV_SYS_LTE   0x10
#define LOC_APN_PROFILE_SRV_SYS_MAX   0x1F


/*PDP type*/
#define LOC_APN_PROFILE_PDN_TYPE_IPV4    0x01
#define LOC_APN_PROFILE_PDN_TYPE_IPV6    0x02
#define LOC_APN_PROFILE_PDN_TYPE_IPV4V6  0x03
#define LOC_APN_PROFILE_PDN_TYPE_PPP     0x04
#define LOC_APN_PROFILE_PDN_TYPE_MAX     0x04



typedef struct {  
  uint32  srv_system_type; /*Serving system on which this APN should be used*/
  uint32  pdp_type; /*Pdp type of the APN profile*/
  uint32  reserved; /*Reserved for future use*/
  uint8   apn_name[LOC_MAX_APN_NAME_LENGTH]; /*APN name*/
} loc_apn_profiles_type;

/******************************************************************************
 * For LOC_IOCTL_SET(GET)_ENGINE_LOCK. The status will be sent back to the client
 * via the callback passed in loc_open with the event set to LOC_EVENT_IOCTL_REPORT.
 * When LOC_IOCTL_GET_ENGINE_LOCK is issued and the ioctl_status indicates that
 * the call is successful, the engine lock status will be available in the event payload.
 *****************************************************************************/
/* Data structure used for this event is loc_lock_e_type */
typedef enum
{
   LOC_LOCK_NONE = 1,
   LOC_LOCK_MI   = 2,
   LOC_LOCK_MT   = 3,
   LOC_LOCK_ALL  = 4,
   LOC_LOCK_MAX  = 0x10000000, /* Force enum to be 32-bit */
} loc_lock_e_type;

/******************************************************************************
 * For LOC_IOCTL_SET(GET)_SBAS_CONFIG
 * The status will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 * When LOC_IOCTL_GET_SBAS_CONFIG is issued and the ioctl_status indicates that
 * the call is successful, the SBAS config will be available in the event payload.
 *****************************************************************************/

/* Data structure used for this event is boolean, true=enable, false=disable */

/******************************************************************************
 * For LOC_IOCTL_SET(GET)_NMEA_TYPES
 * The status will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 * When LOC_IOCTL_GET_NMEA_TYPES is issued and the ioctl_status indicates that
 * the call is successful, the NMEA types will be available in the event payload.
 *****************************************************************************/

/* Data structure used for this event is loc_nmea_sentence_types */
typedef uint32              loc_nmea_sentence_type;

/*================= Interested NMEA string masks ==================*/
#define LOC_NMEA_MASK_ALL    0xffffffff
#define LOC_NMEA_MASK_GGA    0x00000001
#define LOC_NMEA_MASK_RMC    0x00000002
#define LOC_NMEA_MASK_GSV    0x00000004
#define LOC_NMEA_MASK_GSA    0x00000008
#define LOC_NMEA_MASK_VTG    0x00000010
#define LOC_NMEA_MASK_PQXFI  0x00000020
#define LOC_NMEA_MASK_PSTIS  0x00000040
#define LOC_NMEA_MASK_GLGSV  0x00000080
#define LOC_NMEA_MASK_GNGSA  0x00000100
#define LOC_NMEA_MASK_GNGNS  0x00000200
#define LOC_NMEA_MASK_GARMC  0x00000400
#define LOC_NMEA_MASK_GAGSV  0x00000800
#define LOC_NMEA_MASK_GAGSA  0x00001000
#define LOC_NMEA_MASK_GAVTG  0x00002000
#define LOC_NMEA_MASK_GAGGA  0x00004000
#define LOC_NMEA_MASK_PQGSA  0x00008000
#define LOC_NMEA_MASK_PQGSV  0x00010000

/******************************************************************************
 * For LOC_IOCTL_SET(GET)_ON_DEMAND_LPM
 * The status will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 * When LOC_IOCTL_GET_ON_DEMAND_LPM is issued and the ioctl_status indicates that
 * the call is successful, the mode will be available in the event payload.
 *****************************************************************************/

/* Data structure used for this event is boolean, true=enable, false=disable */

/******************************************************************************
 * For LOC_IOCTL_SET(GET)_XXX_SERVER_ADDR
 * The status will be sent back to the client via the callback passed in loc_open
 * with the event set to LOC_EVENT_IOCTL_REPORT.
 * When LOC_IOCTL_GET_XXX_SERVER_ADDR is issued and the ioctl_status indicates that
 * the call is successful, the server address will be available in the event payload.
 * If LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR is caleld without prior call to
 * LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR, then the returned payload will indicate
 * an IPV4 custom server with address and port both set to 0.
 *****************************************************************************/

/* Data structure used for these events is loc_server_info_s_type */

/******************************************************************************
 * For LOC_IOCTL_DELETE_ASSIST_DATA
 *****************************************************************************/


typedef uint64 loc_assist_data_type;
#define LOC_ASSIST_DATA_EPH                     ((loc_assist_data_type)0x00000001)
#define LOC_ASSIST_DATA_ALM                     ((loc_assist_data_type)0x00000002)
#define LOC_ASSIST_DATA_POS                     ((loc_assist_data_type)0x00000004)
#define LOC_ASSIST_DATA_TIME                    ((loc_assist_data_type)0x00000008)
#define LOC_ASSIST_DATA_IONO                    ((loc_assist_data_type)0x00000010)
#define LOC_ASSIST_DATA_UTC                     ((loc_assist_data_type)0x00000020)
#define LOC_ASSIST_DATA_HEALTH                  ((loc_assist_data_type)0x00000040)
#define LOC_ASSIST_DATA_SVDIR                   ((loc_assist_data_type)0x00000080)
#define LOC_ASSIST_DATA_SVSTEER                 ((loc_assist_data_type)0x00000100)
#define LOC_ASSIST_DATA_SADATA                  ((loc_assist_data_type)0x00000200)
#define LOC_ASSIST_DATA_RTI                     ((loc_assist_data_type)0x00000400)
#define LOC_ASSIST_DATA_CELLDB_INFO             ((loc_assist_data_type)0x00000800)
#define LOC_ASSIST_DATA_ALM_CORR_DELETE         ((loc_assist_data_type)0x00001000)
#define LOC_ASSIST_DATA_FREQ_BIAS_EST_DELETE    ((loc_assist_data_type)0x00002000)
#define LOC_ASSIST_DATA_EPH_GLO                 ((loc_assist_data_type)0x00004000)
#define LOC_ASSIST_DATA_ALM_GLO                 ((loc_assist_data_type)0x00008000)
#define LOC_ASSIST_DATA_SVDIR_GLO               ((loc_assist_data_type)0x00010000)
#define LOC_ASSIST_DATA_SVSTEER_GLO             ((loc_assist_data_type)0x00020000)
#define LOC_ASSIST_DATA_ALM_CORR_GLO_DELETE     ((loc_assist_data_type)0x00040000)
#define LOC_ASSIST_DATA_TIME_GPS                ((loc_assist_data_type)0x00080000)
#define LOC_ASSIST_DATA_TIME_GLO                ((loc_assist_data_type)0x00100000)
#define LOC_ASSIST_DATA_SVDIR_SBAS              ((loc_assist_data_type)0x00200000)
#define LOC_ASSIST_DATA_SVSTEER_SBAS            ((loc_assist_data_type)0x00400000)
#define LOC_ASSIST_DATA_CLOCK_INFO              ((loc_assist_data_type)0x00800000)
#define LOC_ASSIST_DATA_SV_NO_EXIST             ((loc_assist_data_type)0x01000000)
#define LOC_ASSIST_DATA_TIME_BDS                ((loc_assist_data_type)0x02000000)
#define LOC_ASSIST_DATA_EPH_BDS                 ((loc_assist_data_type)0x04000000)
#define LOC_ASSIST_DATA_ALM_BDS                 ((loc_assist_data_type)0x08000000)
#define LOC_ASSIST_DATA_SVDIR_BDS               ((loc_assist_data_type)0x10000000)
#define LOC_ASSIST_DATA_SVSTEER_BDS             ((loc_assist_data_type)0x20000000)
#define LOC_ASSIST_DATA_ALM_CORR_BDS_DELETE     ((loc_assist_data_type)0x40000000)
#define LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GPS   ((loc_assist_data_type)0x80000000)
#define LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GLO   ((loc_assist_data_type)0x100000000)
#define LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_BDS   ((loc_assist_data_type)0x200000000)
#define LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GAL   ((loc_assist_data_type)0x400000000)
#define LOC_ASSIST_DATA_TIME_GAL                ((loc_assist_data_type)0x800000000)
#define LOC_ASSIST_DATA_EPH_GAL                 ((loc_assist_data_type)0x1000000000)
#define LOC_ASSIST_DATA_ALM_GAL                 ((loc_assist_data_type)0x2000000000)
#define LOC_ASSIST_DATA_SVDIR_GAL               ((loc_assist_data_type)0x4000000000)
#define LOC_ASSIST_DATA_SVSTEER_GAL             ((loc_assist_data_type)0x8000000000)
#define LOC_ASSIST_DATA_ALM_CORR_GAL_DELETE     ((loc_assist_data_type)0x10000000000)
#define LOC_ASSIST_DATA_EPH_SBAS                ((loc_assist_data_type)0x20000000000)
#define LOC_ASSIST_DATA_ALM_SBAS                ((loc_assist_data_type)0x40000000000)

 #define LOC_ASSIST_DATA_ALL                                                  \
              (      LOC_ASSIST_DATA_EPH |                                    \
                     LOC_ASSIST_DATA_ALM |                                    \
                     LOC_ASSIST_DATA_POS |                                    \
                     LOC_ASSIST_DATA_TIME |                                   \
                     LOC_ASSIST_DATA_IONO |                                   \
                     LOC_ASSIST_DATA_UTC |                                    \
                     LOC_ASSIST_DATA_HEALTH |                                 \
                     LOC_ASSIST_DATA_SVDIR |                                  \
                     LOC_ASSIST_DATA_SVSTEER |                                \
                     LOC_ASSIST_DATA_SADATA |                                 \
                     LOC_ASSIST_DATA_RTI |                                    \
                     LOC_ASSIST_DATA_CELLDB_INFO |                            \
                     LOC_ASSIST_DATA_ALM_CORR_DELETE |                        \
                     LOC_ASSIST_DATA_FREQ_BIAS_EST_DELETE |                   \
                     LOC_ASSIST_DATA_EPH_GLO |                                \
                     LOC_ASSIST_DATA_ALM_GLO |                                \
                     LOC_ASSIST_DATA_SVDIR_GLO |                              \
                     LOC_ASSIST_DATA_SVSTEER_GLO |                            \
                     LOC_ASSIST_DATA_ALM_CORR_GLO_DELETE |                    \
                     LOC_ASSIST_DATA_TIME_GPS |                               \
                     LOC_ASSIST_DATA_TIME_GLO |                               \
                     LOC_ASSIST_DATA_SVDIR_SBAS |                             \
                     LOC_ASSIST_DATA_SVSTEER_SBAS |                           \
                     LOC_ASSIST_DATA_CLOCK_INFO |                             \
                     LOC_ASSIST_DATA_SV_NO_EXIST |                            \
                     LOC_ASSIST_DATA_TIME_BDS |                               \
                     LOC_ASSIST_DATA_EPH_BDS |                                \
                     LOC_ASSIST_DATA_ALM_BDS |                                \
                     LOC_ASSIST_DATA_SVDIR_BDS |                              \
                     LOC_ASSIST_DATA_SVSTEER_BDS |                            \
                     LOC_ASSIST_DATA_ALM_CORR_BDS_DELETE |                    \
                     LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GPS |                  \
                     LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GLO |                  \
                     LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_BDS |                  \
                     LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GAL |                  \
                     LOC_ASSIST_DATA_TIME_GAL |                               \
                     LOC_ASSIST_DATA_EPH_GAL |                                \
                     LOC_ASSIST_DATA_ALM_GAL |                                \
                     LOC_ASSIST_DATA_SVDIR_GAL |                              \
                     LOC_ASSIST_DATA_SVSTEER_GAL |                            \
                     LOC_ASSIST_DATA_ALM_CORR_GAL_DELETE |                    \
                     LOC_ASSIST_DATA_EPH_SBAS |                               \
                     LOC_ASSIST_DATA_ALM_SBAS                                 \
               )


/* These use PDAPI Masks Directly */
#define LOC_CELLDB_DELETE_ALL                                                 \
                   (      PDSM_PA_DELETE_CELLDB_MASK_POS_INFO |               \
                          PDSM_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO |    \
                          PDSM_PA_DELETE_CELLDB_MASK_OTA_POS_INFO |           \
                          PDSM_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO |       \
                          PDSM_PA_DELETE_CELLDB_MASK_TIMETAG_INFO |           \
                          PDSM_PA_DELETE_CELLDB_MASK_CELLID_INFO |            \
                          PDSM_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO |     \
                          PDSM_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO |     \
                          PDSM_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO |      \
                          PDSM_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO       \
                    )

/* These use PDAPI Masks Directly */                    
#define LOC_CLOCK_INFO_DELETE_ALL        (PDSM_PA_DELETE_ALL_CLOCK_INFO)

typedef struct
{
   loc_assist_data_type                     type;
   /* Cell DB Mask, using PDAPI Masks */
   uint32                                   cell_db;
   /* Clock Info Mask, using PDAPI Masks */
   uint32                                   clock_info;
} loc_assist_data_delete_s_type;

/* Structure to define the Secure Get Location report Client  parameters */
typedef struct
{
   /* client which requested the best available  position from specified source */
   loc_client_handle_type     get_secure_available_pos_client_handle;

   /* variable to serialize the available  position request,
      current implementation does not support concurrent requests */
   boolean                    get_secure_available_pos_in_progress;

   /* transaction id for the best available  position from specified source request */
   uint32                     get_secure_available_pos_txn_id;

   /* variable to check if the Nonce provided in best available  position from specified source is valid */
   boolean                    get_secure_available_pos_is_Nonce_valid;

   /* Security Nonce for the available position request */
   uint64                     get_secure_available_pos_Nonce;

   /* variable to check if the Data mode of the Get best Available position  */
   qmiLocSecureLocDataModeEnumT_v02  get_secure_available_pos_data_mode;

  /* variable for Position estimate Source to  Get  Available position  */
  qmiLocPositionSourceEnumT_v02  get_pos_data_source;

   
} loc_secure_get_available_location;

/* -----------------------------------------------------------------------*//**
 @brief 
 Request Message; Used by the control point to get the best available
                    position estimate from the location engine. 
*//* ------------------------------------------------------------------------*/                    
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32 transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned in the Secure Get Best Available Position indication. */

  /* Optional */
  /*  Security Nonce */
  uint8 nonce_valid;  /**< Must be set to true if nonce is being passed */
  uint64 nonce;
  /**<   Identifies the Secure transaction. The nonce value
       is returned in the Secure Get Best Available Position indication. */

  /* Optional */
  /*  Position Source */
  uint8 locPositionSource_valid;  /**< Must be set to true if locPositionSource is being passed */
  qmiLocPositionSourceEnumT_v02 locPositionSource;
  /**<   Position Source.
 Valid values: \n
      - eQMI_LOC_POS_SRC_GNSS (1) --  GNSS Position
 */

  /* Optional */
  /*  Position Report Data mode */
  uint8 locPositionRptDataMode_valid;  /**< Must be set to true if locPositionRptDataMode is being passed */
  qmiLocSecureLocDataModeEnumT_v02 locPositionRptDataMode;
  /**<   Position Report Data Mode.
 If this TLV is not sent, Position report defaults Encryption turned ON.
 Valid values: \n
      - eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED (1) --  Data shall be encrypted 
      - eQMI_LOC_SECURE_LOC_DATA_UNENCRYPTED (2) --  Data shall be un-encrypted 
 */

  /* Optional */
  /*  Secured position report data propagation */
  uint8 locPositionRptDataTimePropogation_valid;  /**< Must be set to true if locPositionRptDataTimePropogation is being passed */
  qmiLocPositionRptPropagationEnumT_v02 locPositionRptDataTimePropogation;
  /**<   Secured position report data propagation
 If this TLV is not sent, Position report is default propagated to current UTC time.
   - Parameter Type : int32 \n
    Parameter Valid values: \n
		- eQMI_LOC_POS_REPORT_PROPAGATED_TO_CURRENT_UTC (1)   --  Position reports are Propagated to current UTC. \n
	*/
}qmiLocGetAvailablePositionReqParamMsgT_v02; 

/* -----------------------------------------------------------------------*//**
 @brief 
 Indication Message; Used by the control point to get the best available
                    position estimate from the location engine. 

*//* ------------------------------------------------------------------------*/                    

typedef struct {

  /* Mandatory */
  /*  Get Best Available Position Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Best Available Position request.

 Valid values: \n
      - eQMI_LOC_SUCCESS (0) --  Request was completed successfully \n 
      - eQMI_LOC_GENERAL_FAILURE (1) --  Request failed because of a general failure \n 
      - eQMI_LOC_UNSUPPORTED (2) --  Request failed because it is not supported \n 
      - eQMI_LOC_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters \n 
      - eQMI_LOC_ENGINE_BUSY (4) --  Request failed because the engine is busy \n 
      - eQMI_LOC_PHONE_OFFLINE (5) --  Request failed because the phone is offline \n 
      - eQMI_LOC_TIMEOUT (6) --  Request failed because it timed out \n 
      - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested \n 
      - eQMI_LOC_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficient memory for the request \n 
      - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) --  Request failed because the maximum number of Geofences are already programmed \n 
      - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) --  Location service failed because of an XTRA version-based file format check failure  */

 
  /* Optional */
  /*  Transaction ID */
  uint8 transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32 transactionId;
  /**<   Transaction ID that was specified in the Get Best
       Available Position request. This parameter will
       always be present if the status field is set to
       SUCCESS. */

  /* Optional */
  /*  Latitude */
  uint8 latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item     Range: -90.0 to 90.0      \begin{itemize1}
         \item    Positive values indicate northern latitude
         \item    Negative values indicate southern latitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Optional */
  /*   Longitude */
  uint8 longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -180.0 to 180.0     \begin{itemize1}
         \item    Positive values indicate eastern longitude
         \item    Negative values indicate western longitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Optional */
  /*   Circular Horizontal Position Uncertainty */
  uint8 horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units: Meters */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8 altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units: Meters \n
       - Range: -500 to 15883 */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8 vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units: Meters */

  /* Optional */
  /*  UTC Timestamp */
  uint8 timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64 timestampUtc;
  /**<   UTC timestamp.
       \begin{itemize1}
       \item Units: Milliseconds since Jan. 1, 1970
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Time Uncertainty */
  uint8 timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Units: Milliseconds */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Minor Axis */
  uint8 horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty. \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Major Axis */
  uint8 horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty. \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Azimuth */
  uint8 horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation. \n
       - Units: Decimal degrees \n
       - Range: 0 to 180 */

  /* Optional */
  /*  Horizontal Circular Confidence */
  uint8 horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8 horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence. \n
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Elliptical Confidence */
  uint8 horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8 horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence. \n
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8 horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Specifies the reliability of the horizontal position.

 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Location reliability is not set 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Location reliability is very low; use it at your own risk 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Location reliability is low; little or no cross-checking is possible 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Location reliability is medium; limited cross-check passed  
      - eQMI_LOC_RELIABILITY_HIGH (4) --  Location reliability is high; strong cross-check passed 
 */

  /* Optional */
  /*  Horizontal Speed */
  uint8 horSpeed_valid;  /**< Must be set to true if horSpeed is being passed */
  float horSpeed;
  /**<   Horizontal speed. \n
       - Units: Meters/second */

  /* Optional */
  /*  Horizontal Speed Uncertainty */
  uint8 horSpeedUnc_valid;  /**< Must be set to true if horSpeedUnc is being passed */
  float horSpeedUnc;
  /**<   Horizontal speed uncertainty. \n
       - Units: Meters/second */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8 altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level. \n
       - Units: Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8 vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8 vertConfidence;
  /**<   Vertical uncertainty confidence. \n
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8 vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Specifies the reliability of the vertical position.

 Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Location reliability is not set 
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Location reliability is very low; use it at your own risk 
      - eQMI_LOC_RELIABILITY_LOW (2) --  Location reliability is low; little or no cross-checking is possible 
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Location reliability is medium; limited cross-check passed  
      - eQMI_LOC_RELIABILITY_HIGH (4) --  Location reliability is high; strong cross-check passed 
 */

  /* Optional */
  /*  Vertical Speed */
  uint8 vertSpeed_valid;  /**< Must be set to true if vertSpeed is being passed */
  float vertSpeed;
  /**<   Vertical speed. \n
         - Units: Meters/second */

  /* Optional */
  /*  Vertical Speed Uncertainty */
  uint8 vertSpeedUnc_valid;  /**< Must be set to true if vertSpeedUnc is being passed */
  float vertSpeedUnc;
  /**<   Vertical speed uncertainty. \n
       - Units: Meters/second */

  /* Optional */
  /*  Heading */
  uint8 heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading. \n
         - Units: Degrees \n
         - Range: 0 to 359.999  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8 headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty. \n
       - Type: Floating point \n
       - Range: 0 to 359.999 */

  /* Optional */
  /*  Magnetic Deviation */
  uint8 magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. */

  /* Optional */
  /*  Technology Used Mask */
  uint8 technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.
 Valid bitmasks: \n
      - QMI_LOC_POS_TECH_MASK_SATELLITE (0x00000001) --  Satellites were used to generate the fix 
      - QMI_LOC_POS_TECH_MASK_CELLID (0x00000002) --  Cell towers were used to generate the fix 
      - QMI_LOC_POS_TECH_MASK_WIFI (0x00000004) --  Wi-Fi access points were used to generate the fix 
      - QMI_LOC_POS_TECH_MASK_SENSORS (0x00000008) --  Sensors were used to generate the fix 
      - QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION (0x00000010) --  Reference Location was used to generate the fix 
      - QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION (0x00000020) --  Coarse position injected into the location engine was used to
        generate the fix 
      - QMI_LOC_POS_TECH_MASK_AFLT (0x00000040) --  AFLT was used to generate the fix 
      - QMI_LOC_POS_TECH_MASK_HYBRID (0x00000080) --  GNSS and network-provided measurements were used to
        generate the fix 
 */

  /* Optional */
  /*  Dilution of Precision */
  uint8 DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;

  /* Optional */
  /*  GPS Time */
  uint8 gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;

  /* Optional */
  /*  Time Source */
  uint8 timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Time source.
 Valid values: \n
      - eQMI_LOC_TIME_SRC_INVALID (0) --  Invalid time. 
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER (1) --  Time is set by the 1X system 
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING (2) --  Time is set by WCDMA/GSM time tagging (i.e.,
       associating network time with GPS time) 
      - eQMI_LOC_TIME_SRC_EXTERNAL_INPUT (3) --  Time is set by an external injection 
      - eQMI_LOC_TIME_SRC_TOW_DECODE (4) --  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite 
      - eQMI_LOC_TIME_SRC_TOW_CONFIRMED (5) --  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites 
      - eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED (6) --  Both time of the week and the GPS week number are known 
      - eQMI_LOC_TIME_SRC_NAV_SOLUTION (7) --  Time is set by the position engine after the fix is obtained 
      - eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME (8) --  Time is set by the position engine after performing SFT;
       this is done when the clock time uncertainty is large 
      - eQMI_LOC_TIME_SRC_GLO_TOW_DECODE (9) --  Time is set after decoding GLO satellites 
      - eQMI_LOC_TIME_SRC_TIME_TRANSFORM (10) --  Time is set after transforming the GPS to GLO time 
      - eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING (11) --  Time is set by the sleep time tag provided by the WCDMA network 
      - eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING (12) --  Time is set by the sleep time tag provided by the GSM network 
      - eQMI_LOC_TIME_SRC_UNKNOWN (13) --  Source of the time is unknown 
      - eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK (14) --  Time is derived from the system clock (better known as the slow clock);
       GNSS time is maintained irrespective of the GNSS receiver state 
      - eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE (15) --  Time is set after decoding QZSS satellites 
      - eQMI_LOC_TIME_SRC_BDS_TOW_DECODE (16) --  Time is set after decoding BDS satellites 
 */

  /* Optional */
  /*  Sensor Data Usage */
  uint8 sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  qmiLocSensorUsageIndicatorStructT_v02 sensorDataUsage;

  /* Optional */
  /*  SVs Used to Calculate the Fix */
  uint8 gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32 gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16 gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
       used for calculating this position report. The following
       information is associated with each SV ID: \n
       Range: \n
       - For GPS:     1 to 32 \n
       - For SBAS:    33 to 64 \n
       - For GLONASS: 65 to 96 \n
       - For QZSS:    193 to 197 \n
       - For BDS:     201 to 237
    */

  /* Optional */
  /*  NONCE */
  uint8 nonce_valid;  /**< Must be set to true if nonce is being passed */
  uint64 nonce;

  /* Optional */
  /* variable to check  the Data mode of the Get Available Secure position report  */
  qmiLocSecureLocDataModeEnumT_v02  get_secure_available_pos_data_mode;
  
}qmiLocGetAvailablePositionIndParamMsgT_v02; 
/******************************************************************************
 *
 *                         loc_ioctl function call data
 *
 *****************************************************************************/
typedef union
{
  loc_fix_criteria_s_type                           fix_criteria;
  loc_user_verify_s_type                            user_verify_resp;
  loc_predicted_orbits_data_s_type                  predicted_orbits_data;
  loc_predicted_orbits_auto_download_config_s_type  predicted_orbits_auto_download;
  loc_assist_data_time_s_type                       assistance_data_time;
  loc_assist_data_pos_s_type                        assistance_data_position;
  loc_server_open_status_s_type                     conn_open_status;
  loc_server_close_status_s_type                    conn_close_status;
  loc_lock_e_type                                   engine_lock;
  boolean                                           sbas_mode;
  loc_nmea_sentence_type                            nmea_types;
  boolean                                           on_demand_lpm;
  loc_server_info_s_type                            server_addr;
  loc_assist_data_delete_s_type                     assist_data_delete;
  loc_efs_data_s_type                               efs_data;            /* For EFS operations */
  loc_error_estimate_config_e_type                  error_estimate_config;
  uint8                                             xtra_t_session_control; /* for XTRA-T session control */
  loc_apn_profiles_type                             apn_profiles[LOC_MAX_APN_PROFILES]; /*APN profiles used by PDCOMM*/
  boolean                                           data_enable; /*enable/disable data*/
  uint32                                            supl_version; /* SUPL version(Bytes from MSB to LSB: 
                                                                    reserved, major, minor, serviceIndicator) */
  loc_server_multi_open_status_s_type               multi_conn_open_status;                                                                     
  loc_event_mask_type                               event_reg_mask; /*< used  to change event registration mask for the calling client */
  qmiLocSetOperationModeReqMsgT_v02                 operation_mode; /*< used to update the operation mode for the calling client */
  boolean                                           supl_security; /*< used to update SUPL security If TRUE: SUPL, else: UPL */
  qmiLocVxVersionEnumT_v02                          vx_config; /*< used to update VX Config */
  qmiLocSetPositionEngineConfigParametersReqMsgT_v02 z_SetPeConfigReq; /*< Used to send the PE config */                                                                 
  qmiLocGetPositionEngineConfigParametersReqMsgT_v02 z_GetPeConfigReq; /*< Used to get the PE Config */
  qmiLocLppConfigMaskT_v02                          lpp_config; /* LPP configuration */
  qmiLocGetBestAvailablePositionReqMsgT_v02         bap_req; /* best available position request*/
  qmiLocAssistedGlonassProtocolMaskT_v02            assisted_glo_config; /* assisted glo configuration */
  qmiLocInjectGSMCellInfoReqMsgT_v02                inject_gsm_cell_info; /* GSM Cell Information */
  qmiLocInjectNetworkInitiatedMessageReqMsgT_v02    inject_ni_msg;  /* external NI msg to be injected */
  qmiLocEmergencyProtocolEnumT_v02                  emergency_protocol; /* emergency protocol configuration*/
  qmiLocSuplHashAlgoEnumT_v02                       supl_hash_algo; /* SUPL hash algorithm*/
  qmiLocSuplTlsVersionEnumT_v02                     supl_tls_version; /* SUPL TLS version */
  qmiLocGetAvailWwanPositionReqMsgT_v02             z_GetAvailWwanPosReq; /* available WWAN position request */
  qmiLocGetAvailablePositionReqParamMsgT_v02        z_GetAvailablePosReq; /* available Position estimate request */
  qmiLocSetPremiumServicesCfgReqMsgT_v02            z_SetPremSvcsCfgReq;  /* premium services config request */
  uint8                                             wifi_scan_inject_timeout; /* timeout duration pdsm wait for wifi scan results after the event notification is sent */
  qmiLocXtraVersionCheckEnumT_v02                   z_XtraVersion; /* Xtra version check configuration */
  loc_reserved_payload_s_type                       reserved;
  qmiLocSetGNSSConstRepConfigReqMsgT_v02            z_GNSS_config; /*GNSS constellation report configuration for measurement and SV poly */  
  qmiLocDeleteGNSSServiceDataReqMsgT_v02            z_DelGNSSSvcDataReq; /* Delete GNSS service data */
} loc_ioctl_data_u_type;
/*~ CASE          LOC_IOCTL_SET_FIX_CRITERIA             loc_ioctl_data_u_type.fix_criteria */
/*~ CASE          LOC_IOCTL_INFORM_NI_USER_RESPONSE     loc_ioctl_data_u_type.user_verify_resp */
/*~ CASE          LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA loc_ioctl_data_u_type.predicted_orbits_data */
/*~ CASE          LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD         loc_ioctl_data_u_type.predicted_orbits_auto_download */
/*~ CASE          LOC_IOCTL_INJECT_UTC_TIME                  loc_ioctl_data_u_type.assistance_data_time */
/*~ CASE          LOC_IOCTL_INJECT_POSITION              loc_ioctl_data_u_type.assistance_data_position */
/*~ CASE          LOC_IOCTL_INFORM_SERVER_OPEN_STATUS  loc_ioctl_data_u_type.conn_open_status */
/*~ CASE          LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS loc_ioctl_data_u_type.conn_close_status */
/*~ CASE          LOC_IOCTL_SET_ENGINE_LOCK              loc_ioctl_data_u_type.engine_lock */
/*~ CASE          LOC_IOCTL_ERROR_ESTIMATE_CONFIG        loc_ioctl_data_u_type.error_estimate_config */
/*~ IF  (LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR == _DISC_ ||
         LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR == _DISC_ ||
         LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR == _DISC_ ||
         LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR == _DISC_)   loc_ioctl_data_u_type.server_addr */
/*~ CASE          LOC_IOCTL_SET_SBAS_CONFIG              loc_ioctl_data_u_type.sbas_mode */
/*~ CASE          LOC_IOCTL_SET_NMEA_TYPES               loc_ioctl_data_u_type.nmea_types */
/*~ CASE          LOC_IOCTL_SET_ON_DEMAND_LPM            loc_ioctl_data_u_type.on_demand_lpm */
/*~ CASE          LOC_IOCTL_DELETE_ASSIST_DATA           loc_ioctl_data_u_type.assist_data_delete */
/*~ CASE          LOC_IOCTL_ACCESS_EFS_DATA              loc_ioctl_data_u_type.efs_data */
/*~ CASE          LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL   loc_ioctl_data_u_type.xtra_t_session_control */
/*~ IF  (LOC_IOCTL_SET_LBS_APN_PROFILE == _DISC_ ||
         LOC_IOCTL_SET_XTRA_APN_PROFILE == _DISC_)       loc_ioctl_data_u_type.apn_profiles */
/*~ CASE          LOC_IOCTL_SET_DATA_ENABLE              loc_ioctl_data_u_type.data_enable */
/*~ CASE          LOC_IOCTL_SET_SUPL_VERSION             loc_ioctl_data_u_type.supl_version */
/*~ CASE          LOC_IOCTL_INFORM_SERVER_MULTI_OPEN_STATUS  loc_ioctl_data_u_type.multi_conn_open_status */
/*~ CASE          LOC_IOCTL_SET_EVENT_REG_MASK           loc_ioctl_data_u_type.event_reg_mask */
/*~ CASE          LOC_IOCTL_SET_OPERATION_MODE           loc_ioctl_data_u_type.operation_mode */
/*~ CASE          LOC_IOCTL_SET_SUPL_SECURITY            loc_ioctl_data_u_type.supl_security */
/*~ CASE          LOC_IOCTL_SET_VX_CONFIG                loc_ioctl_data_u_type.vx_config */
/*~ CASE          LOC_IOCTL_SET_PE_CONFIG                loc_ioctl_data_u_type.z_SetPeConfigReq */
/*~ CASE          LOC_IOCTL_GET_PE_CONFIG                loc_ioctl_data_u_type.z_GetPeConfigReq */
/*~ CASE          LOC_IOCTL_SET_LPP_CONFIG               loc_ioctl_data_u_type.lpp_config */
/*~ CASE          LOC_IOCTL_GET_BEST_AVAILABLE_POSITION  loc_ioctl_data_u_type.bap_req */
/*~ CASE          LOC_IOCTL_SET_ASSISTED_GLO_CONFIG      loc_ioctl_data_u_type.assisted_glo_config */
/*~ CASE          LOC_IOCTL_SET_EMERGENCY_PROTOCOL       loc_ioctl_data_u_type.emergency_protocol */
/*~ CASE          LOC_IOCTL_SET_SUPL_HASH_ALGO           loc_ioctl_data_u_type.supl_hash_algo */
/*~ CASE          LOC_IOCTL_SET_SUPL_TLS_VERSION         loc_ioctl_data_u_type.supl_tls_version */
/*~ CASE          LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION  loc_ioctl_data_u_type.z_GetAvailWwanPosReq */
/*~ CASE          LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC  loc_ioctl_data_u_type.z_GetAvailablePosReq */
/*~ CASE          LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG  loc_ioctl_data_u_type.z_SetPremSvcsCfgReq */
/*~ CASE          LOC_IOCTL_SET_XTRA_VERSION_CHECK       loc_ioctl_data_u_type.z_XtraVersion */
/*~ CASE          LOC_IOCTL_RESERVED_CMD                 loc_ioctl_data_u_type.reserved */
/*~ DEFAULT loc_ioctl_data_u_type.void */



/******************************************************************************
 *
 *   IOCTL callback data (applicable for every IOCTL call)
 *
 *****************************************************************************/

/* Structure to be filled for callback data of LOC_IOCTL_ACCESS_EFS_DATA */
typedef struct
{
  
  loc_fs_operation_e_type             efs_operation;  
}loc_ioctl_access_efs_callback_data_type;

/* Union of all ioctl callback data */
typedef union
{
   loc_api_version_s_type             api_version;
   loc_fix_criteria_s_type            fix_criteria;
   loc_lock_e_type                    engine_lock;
   boolean                            sbas_mode;
   loc_nmea_sentence_type             nmea_types;
   boolean                            on_demand_lpm;
   loc_server_info_s_type             server_addr;/* For AGPS, UMTS, and MPC */
   loc_predicted_orbits_data_source_s_type            predicted_orbits_data_source;
   loc_predicted_orbits_data_validity_report_s_type   predicted_orbits_data_validity;
   uint8                                              xtra_t_session_control; /* for XTRA-T session control */
   loc_apn_profiles_type              apn_profiles[LOC_MAX_APN_PROFILES]; /*APN profiles used by PDCOMM*/
   uint32                             supl_version; /* SUPL version(Bytes from MSB to LSB: 
                                                       reserved, major, minor, serviceIndicator) */
   qmiLocOperationModeEnumT_v02        operation_mode;  
   loc_event_mask_type                 event_reg_mask;    
   uint16                              w_PredOrbitsPartNum;
   boolean                             supl_security; 
   qmiLocVxVersionEnumT_v02            vx_config; 
   loc_ioctl_access_efs_callback_data_type z_AccessEfsData;
   qmiLocSetPositionEngineConfigParametersIndMsgT_v02 z_SetPeConfigInd;
   qmiLocGetPositionEngineConfigParametersIndMsgT_v02 z_GetPeConfigInd;
   qmiLocLppConfigMaskT_v02            lpp_config;
   qmiLocGetBestAvailablePositionIndMsgT_v02 bap_ind;
   qmiLocAssistedGlonassProtocolMaskT_v02    assisted_glo_config;
   qmiLocEmergencyProtocolEnumT_v02          emergency_protocol;
   qmiLocSuplHashAlgoEnumT_v02               supl_hash_algo; /* SUPL hash algorithm*/
   qmiLocSuplTlsVersionEnumT_v02             supl_tls_version; /* SUPL TLS version */
   uint8                                     wifi_scan_inject_timeout; /* timeout duration pdsm wait for wifi scan results after the event notification is sent */
   qmiLocGetAvailWwanPositionIndMsgT_v02     z_GetAvailWwanPosInd;
   qmiLocGetAvailablePositionIndParamMsgT_v02 z_GetAvailablePosInd;
   qmiLocSetPremiumServicesCfgIndMsgT_v02    z_SetPremSvcsCfgInd;
   qmiLocSetXtraVersionCheckIndMsgT_v02      z_SetXtraVerCheckInd;
} loc_ioctl_callback_data_u_type;
/*~ CASE LOC_IOCTL_GET_API_VERSION                      loc_ioctl_callback_data_u_type.api_version */
/*~ CASE LOC_IOCTL_GET_FIX_CRITERIA                     loc_ioctl_callback_data_u_type.fix_criteria */
/*~ CASE LOC_IOCTL_GET_ENGINE_LOCK                      loc_ioctl_callback_data_u_type.engine_lock */
/*~ IF  (LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR == _DISC_ ||
         LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR == _DISC_ ||
         LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR == _DISC_ ||
         LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR == _DISC_)  loc_ioctl_callback_data_u_type.server_addr */
/*~ CASE LOC_IOCTL_GET_SBAS_CONFIG                      loc_ioctl_callback_data_u_type.sbas_mode */
/*~ CASE LOC_IOCTL_GET_NMEA_TYPES                       loc_ioctl_callback_data_u_type.nmea_types */
/*~ CASE LOC_IOCTL_GET_ON_DEMAND_LPM                    loc_ioctl_callback_data_u_type.on_demand_lpm */
/*~ CASE LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE  loc_ioctl_callback_data_u_type.predicted_orbits_data_source */
/*~ CASE LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY loc_ioctl_callback_data_u_type.predicted_orbits_data_validity */
/*~ CASE LOC_IOCTL_GET_XTRA_T_SESSION_CONTROL           loc_ioctl_callback_data_u_type.xtra_t_session_control */
/*~ IF  (LOC_IOCTL_GET_LBS_APN_PROFILE == _DISC_ ||
         LOC_IOCTL_GET_XTRA_APN_PROFILE == _DISC_)      loc_ioctl_callback_data_u_type.apn_profiles */
/*~ CASE LOC_IOCTL_GET_SUPL_VERSION                     loc_ioctl_callback_data_u_type.supl_version */
/*~ CASE LOC_IOCTL_GET_EVENT_REG_MASK                   loc_ioctl_callback_data_u_type.event_reg_mask */
/*~ CASE LOC_IOCTL_GET_OPERATION_MODE                   loc_ioctl_callback_data_u_type.operation_mode */
/*~ CASE LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA         loc_ioctl_callback_data_u_type.w_PredOrbitsPartNum */
/*~ CASE LOC_IOCTL_GET_SUPL_SECURITY                    loc_ioctl_callback_data_u_type.supl_security */
/*~ CASE LOC_IOCTL_GET_VX_CONFIG                        loc_ioctl_callback_data_u_type.vx_config */
/*~ CASE LOC_IOCTL_ACCESS_EFS_DATA                      loc_ioctl_callback_data_u_type.z_AccessEfsData */
/*~ CASE LOC_IOCTL_SET_PE_CONFIG                        loc_ioctl_data_u_type.z_SetPeConfigResp */
/*~ CASE LOC_IOCTL_GET_PE_CONFIG                        loc_ioctl_data_u_type.z_GetPeConfigResp */
/*~ CASE LOC_IOCTL_GET_LPP_CONFIG                       loc_ioctl_callback_data_u_type.lpp_config */
/*~ CASE LOC_IOCTL_GET_BEST_AVAILABLE_POSITION          loc_ioctl_callback_data_u_type.bap_ind */
/*~ CASE LOC_IOCTL_GET_ASSISTED_GLO_CONFIG              loc_ioctl_callback_data_u_type.assisted_glo_config */
/*~ CASE LOC_IOCTL_GET_EMERGENCY_PROTOCOL               loc_ioctl_callback_data_u_type.emergency_protocol */
/*~ CASE LOC_IOCTL_GET_SUPL_HASH_ALGO                   loc_ioctl_data_u_type.supl_hash_algo */
/*~ CASE LOC_IOCTL_GET_SUPL_TLS_VERSION                 loc_ioctl_data_u_type.supl_tls_version */
/*~ CASE LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION          loc_ioctl_callback_data_u_type.z_GetAvailWwanPosInd */
/*~ CASE LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC          loc_ioctl_callback_data_u_type.z_GetAvailablePosInd */
/*~ CASE LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG          loc_ioctl_callback_data_u_type.z_SetPremSvcsCfgInd */
/*~ DEFAULT loc_ioctl_callback_data_u_type.void */

typedef struct
{
   loc_ioctl_e_type                type;   /* IOCTL type for this callback */
   int32                           status; /* IOCTL status, defined in LOC_API_SUCCESS and etc */
   loc_ioctl_callback_data_u_type  data;
} loc_ioctl_callback_s_type;
/*~ FIELD loc_ioctl_callback_s_type.data DISC loc_ioctl_callback_s_type.type */

/******************************************************************************
 *
 *           Union of all location-event request and callback data
 *
 *****************************************************************************/
typedef union
{
  loc_parsed_position_s_type          parsed_location_report;
  loc_gnss_info_s_type                gnss_report;
  loc_nmea_report_s_type              nmea_report;
  loc_ni_event_s_type                 ni_request;
  loc_assist_data_request_s_type      assist_data_request;
  loc_server_request_s_type           loc_server_request;
  loc_ioctl_callback_s_type           ioctl_report;
  loc_status_event_s_type             status_report;
  pdsm_pd_qwip_data_s_type            z_pdQwip;
  loc_gnss_meas_s_type                gnss_meas_report;
  loc_sv_poly_s_type                  sv_poly_report;
  loc_reserved_payload_s_type         reserved;
} loc_event_payload_u_type;
/*~ CASE          LOC_EVENT_PARSED_POSITION_REPORT       loc_event_payload_u_type.parsed_location_report */
/*~ CASE          LOC_EVENT_SATELLITE_REPORT             loc_event_payload_u_type.gnss_report */
/*~ CASE          LOC_EVENT_NMEA_POSITION_REPORT  LOC_EVENT_NMEA_1HZ_REPORT       loc_event_payload_u_type.nmea_report */
/*~ CASE          LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST     loc_event_payload_u_type.ni_request */
/*~ CASE          LOC_EVENT_ASSISTANCE_DATA_REQUEST      loc_event_payload_u_type.assist_data_request */
/*~ CASE          LOC_EVENT_LOCATION_SERVER_REQUEST      loc_event_payload_u_type.loc_server_request */
/*~ CASE          LOC_EVENT_IOCTL_REPORT                 loc_event_payload_u_type.ioctl_report */
/*~ CASE          LOC_EVENT_STATUS_REPORT                loc_event_payload_u_type.status_report */
/*~ CASE          LOC_EVENT_WPS_NEEDED_REQUEST           loc_event_payload_u_type.z_pdQwip */
/*~ CASE          LOC_EVENT_RESERVED                     loc_event_payload_u_type.reserved */
/*~ DEFAULT       loc_event_payload_u_type.void */

/******************************************************************************
 *  Location event callback function type
 *****************************************************************************/
typedef int32 (loc_event_cb_f_type)
(
      // The loc client for this event, only client registered for this event will be called
      loc_client_handle_type        loc_handle,

      loc_event_mask_type           loc_event,

      const loc_event_payload_u_type*  loc_event_payload
      /*~ PARAM loc_event_payload POINTER DISC loc_event */
);
/*~ CALLBACK loc_event_cb_f_type */

/*===========================================================================
 *
 *                          FUNCTION DECLARATION
 *
 *==========================================================================*/

/*===========================================================================
FUNCTION loc_open2

DESCRIPTION
  This function connects a location client to the location engine.
  If connection is successful, it will return a handle which the location client
  should use for future location operations. If connection fails, the return
  value will be LOC_CLIENT_HANDLE_INVALID.

  This is different from loc_open2, as it does not require the event-
  registration mask.

PARAMETERS
  event_callback:          a function pointer which will be invoked by the GPS
                           server when an event of interest happens.

DEPENDENCIES

RETURN VALUE
  -1 (LOC_CLIENT_HANDLE_INVALID) if connection fails.
  Otherwise, a number >= 0.

SIDE EFFECTS

===========================================================================*/

loc_client_handle_type loc_open2
(
   loc_event_cb_f_type    *event_callback
);
/*~ FUNCTION loc_open
     RELEASE_FUNC loc_close(_RESULT_) */

/*===========================================================================
FUNCTION loc_close

DESCRIPTION
  This function discconect the client from the location engine.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
extern int32 loc_close
(
      loc_client_handle_type handle
);
/*~ FUNCTION loc_close */

/*===========================================================================
FUNCTION loc_start_fix

DESCRIPTION
  This function starts positioning process. If client is currently requesting
  fixes, this function will be silently ignored and LOC_API_SUCCESS will be
  returned

  When position is available, position result will be passed to client via
  the callback funtion pointer provided in loc_open.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern int32 loc_start_fix
(
      loc_client_handle_type handle
);
/*~ FUNCTION loc_start_fix  */

/*===========================================================================
FUNCTION loc_stop_fix

DESCRIPTION
  This function stops positioning process for this client. If client is not
  currently requesting fixes, it will return LOC_API_SUCCESS.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
extern int32 loc_stop_fix
(
      loc_client_handle_type handle
);
/*~ FUNCTION loc_stop_fix */

/*===========================================================================
FUNCTION loc_ioctl

DESCRIPTION
  This function is used to control the location engine and retrieve information
  from the engine.

  If loc_ioctl returns LOC_API_SUCCESS, then a callback event through the registered
  callback in loc_open will be fired to notify the final processing result. The event
  will be set to LOC_EVENT_IOCTL_REPORT and the event payload shall be interpreted
  as loc_ioctl_callback_s_type. The type in this payload will indicate the IOCTL
  type. The ioctl_status will indicate whether the loc_ioctl call is successful or not.
  If the loc_ioctl is used to retrieve information from the engine and the ioctl_status
  indicates success, then the requesting payload will be available in the callback.

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
extern int32 loc_ioctl
(
      loc_client_handle_type           handle,
      loc_ioctl_e_type                 ioctl_type,
      const loc_ioctl_data_u_type*     ioctl_data
      /*~ PARAM ioctl_data POINTER DISC ioctl_type */
);
/*~ FUNCTION loc_ioctl */



#ifdef __cplusplus
}
#endif

#endif /* LOC_API_2_H */

  
