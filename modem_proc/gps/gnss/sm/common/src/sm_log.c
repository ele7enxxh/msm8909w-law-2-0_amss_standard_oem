/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   LOCATION SERVICES MANAGER LOGGING MODULE

GENERAL DESCRIPTION
  This file contains LSM log record definitions

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_log.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/15   mj       Assign struct instead of copying individual fields in function sm_log_eph_download()
09/13/15   muk     OEMDRE Log V2 extension
04/17/15   jv      Added QZSS Support
08/27/14   jv      16-bit SVID & IODE Support
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos)
08/28/12   rk      CR 392443 - Avoid mem leak in the event of unsupported PDSM PA event logging.
                   Also fixed bitmask to enum conversion for successful PDSM PA event logging.
06/11/12   rk      Enhance sm_log_pa_event_callback with new PA event enum based assignment.
04/23/12   ssu     Adding sm_log_get_best_avail_pos_info, sm_log_report_ext_status_best_avail_pos_info
03/12/12   rh      Added version 2 for CPI log
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/21/09   rb      added log for Eph download
7/27/09    gk      add a reason in update failure. fixed ipv6 address
07/25/09   atien   sm_log_diag_position() - initialize position source before use
07/25/09   atien   PDSM_PD_EVENT_UPDATE_FAILURE - report event payload with failure reason
5/14/09    gk      fixed log version for the gps state
04/02/09   ns      support for Qwip logs.
04/01/09   atien   Fix critical lint error
02/26/09   atien   Make sure to clear XTRA-T log fields before use
02/04/09   atien   MOLR vs AD call flow support
01/16/09   atien   XTRA-T support
07/29/08   gk      On-demand log
07/28/08   gk      Fix Klocwork errors
06/05/08   lt      Add PD_EVENTs(14-19) that's been in design doc but not here.
07/25/08   gk      fix a log which incorrectly defined
06/12/08   lt     Support for external coarse position injection.
06/09/08   gk      On-Demand Changes to add some logs and events
05/20/08   gk      On-demand changes
09/14/07   gk      NMEA CONFIG set parameters support
7/13/07    gk      umts noticafication not logged correctly. 
5/8/07     gk      position source not initialzed properly. 
4/25/07    gk      modified a pa log to match the new  structure
4/16/07    gk      renumbered delete params 
2/14/07    gk      changed UMTS/Supl notification flags to uint16 
2/06/07    gk      Fixed notification parameters logs, lint errors, warnings 
1/11/07    gk      Fixed set parameters logs, ip address was incorrect in get pos
12/06/06   gk      Check null pointers
12/04/06   gk      Updated position report log to reflect the spec, minor mods
11/8/06    gk      Initial checkin for converged GPS

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sm_log.h"
#include "pdapi.h"
#include "pdapiclient.h"
#include "sm_api.h"
#include "gnss_common.h"
#include "aries_os_api.h"
#include "sm_util.h"

#define SM_MEMCPY(dst,src) memscpy((void*)&(dst), sizeof(dst), (void*)&(src), sizeof(src))
#define SM_MEMCLR(a)   memset((void*)&(a), 0, sizeof(a))

#define CGPS_SM_LOG_PKT_VERSION_1 1
#define CGPS_SM_LOG_PKT_VERSION_2 2
#define CGPS_SM_LOG_PKT_VERSION_3 3
#define CGPS_SM_LOG_PKT_VERSION_4 4
#define CGPS_SM_LOG_PKT_VERSION_5 5
#define CGPS_SM_LOG_PKT_VERSION_6 6
#define CGPS_SM_LOG_PKT_VERSION_7 7

LOG_RECORD_DEFINE( LOG_CGPS_REP_SVR_TX_LOG_PACKET_C )
  uint8         u_Link;     /* link type */
  uint8         u_Protocol; /* protocol type */
  uint8         u_SeqNum;   /* sequence number increments for each log message */
  uint8         u_Flags;    /* flags for segment indication
                          *  b0: 1=first segment, 0=not first segment
                          *  b1: 1=more segments, 0=no more segments */
  uint16        w_Count;    /* If first segment flag=1, contains total len of msg
                          * If first segment flag=0, contains the offset into
                          *   message where this segments starts; begins at 0 */
  uint8         u_Data[1];  /* variable length data of actual bytes sent; 
                          * this declaration is merely a place holder */
LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_CGPS_REP_SVR_RX_LOG_PACKET_C )
  uint8         u_Link;     /* link type */
  uint8         u_Protocol; /* protocol type */
  uint8         u_SeqNum;   /* sequence number increments for each log call */
  uint8         u_Flags;    /* flags for segment indication
                          *  b0: 1=first segment, 0=not first segment
                          *  b1: 1=more segments, 0=no more segments */
  uint16        w_Count;    /* If first segment flag=1, contains total len of msg
                          * If first segment flag=0, contains the offset into
                          *   message where this segments starts; begins at 0 */
  uint8         u_Data[1]; /* variable length data of actual bytes sent; 
                         * this declaration is merely a place holder */
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_CGPS_REP_EVT_LOG_PACKET_C )
  uint8         u_EventId;   /* ID for the LSM specific event */
  uint8         u_EventData; /* Data associated with the event ID */
  uint8         u_PacketVersion; /* Reserved for future use */
  uint8         u_Reserved2; /* Reserved for future use */
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_CGPS_PDSM_GET_POSITION_C )
  uint32 q_ClientId;
  uint16 w_ServiceType;
  uint8 u_SessionType;
  uint8 u_OperationMode;
  uint8 u_ServerOption;
  uint8 u_DatadownloadMode;
  uint16 w_DatadownloadDuration;
  uint32 q_ServerAddress;
  uint32 q_PortId;
  uint32 q_NumFixes;
  uint32 q_TimebetFixes;
  uint32 q_AccuracyThreshold;
  uint8 u_PrivacyLevel;
  uint8 u_GpsSessionTimeout;
  uint8 u_Reserved[8];
  uint8 u_PacketVersion;
  uint8 u_ServerAddrType;
  uint8 q_ServerAddress_v4v6[16];
  uint32 q_ServerPort_v4v6;
  uint32 q_ClassId;
  uint8 u_UrlLength;
  char  u_ServerURL[255];
  uint8  u_Reserved2[16];
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_CGPS_PDSM_ON_DEMAND_SESSION_START_C)
  uint8  u_PktVersion;
  uint8  u_GpsSessionTimeout;
  uint32 q_Accuracy;
  uint32 f_Punc;
  uint32 f_Tunc;
  uint8  u_OnDemandMode;
  uint32 q_Reserved[4];
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_CGPS_PDSM_ON_DEMAND_SESSION_STOP_C )
  uint8  u_PktVersion;
  uint8  u_OnDemandMode;
  uint8  u_Reason;
  uint32 q_NumFailedFixes;
  uint32 q_Reserved[4];
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_CGPS_PDSM_ON_DEMAND_SESSION_NOT_STARTED_C)
  uint8  u_PktVersion;
  uint8  u_OnDemandMode;
  uint8  u_Reason;
  uint32 q_NumFailedFixes;
  uint32 f_Punc;
  uint32 f_Tunc;
  uint32 q_Reserved[4];
LOG_RECORD_END

#if 0
LOG_RECORD_DEFINE( LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C )
  uint8  u_PktVersion;
  uint32 q_ClientId;
  uint32 q_opt_field_mask; 
  uint32 q_time_info_type;
  uint32 q_timestamp_sec; 
  uint32 q_alt_def; 
  uint32 q_source;
  int32  l_latitude;  /* Lat.: rad. LSB = pi/(2^32), 2's comp. */
  int32  l_longitude; /* Long.: rad, LSB = pi /(2^31), 2's comp. */ 
  uint32 l_loc_unc_hor;   /* horizontal location unc.: meters */
  int16  x_altitude; /* alt: meters, positive height, negative depth  */ 
  uint16 w_loc_unc_vert;   /* loc unc vertical: meters */
  uint8  u_confidence_hor;  /* horizontal confidence */
  uint8  u_confidence_vert; /* vertical confidence */
  uint32 q_Reserved[4];
LOG_RECORD_END
#endif 

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_SET_PARAMETERS_C)
  uint8  u_PacketVersion;  
  uint32 q_ClientId;
  uint8 u_ParameterType;
  uint8 u_Length;
  //sm_log_pdsm_set_parameters_value  z_ParameterValue;
  uint8 u_ParameterValue[255];
  uint8  u_Reserved[3];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_END_SESSION_C)
  uint32 q_ClientId;
  uint8 u_SessionType;
  uint8 u_Reserved[3];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C)
  uint8  u_PktVersion;
  uint32 q_ClientId;
  uint8  u_NotifyStatus;
  uint8  u_NotifyType;
  uint16  q_NotifyLength;
  sm_log_nofity_verify_data z_NotifyData;
  uint32 q_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C)
  uint8  u_PktVersion;
  uint32 q_ClientId;
  int32 l_Latitude;
  int32 l_Longitude;
  uint32 q_TimeStamp;
  uint8 u_LocUncAng;
  uint8 u_LocUncA;
  uint8 u_LocUncP;
  uint8 u_OptFieldMask;
  int16 x_Altitude;
  uint16 w_Heading;
  uint16 w_VelocityHor;
  uint8 u_FixType;
  int8 s_VelocityVer;
  uint8 u_LocUncertaintyV;
  uint8 u_PositionMethod;
  uint8 u_PositionSource;
  uint8 u_EndSessionCause;
  uint8 u_UncEllConfidence;
  int8 s_utcOffset;
  int32 q_ExtLoclat;
  int32 q_ExtLoclon;
  uint16 w_ExtLocUncA;
  uint16 w_ExtLocUncP;
  int32 q_VelHoriz;
  int32 q_VelVert;
  uint32 q_TimeStampTowMs;
  uint16 w_TimeStampGpsWeek;
  uint8 u_PosReportedToNetwork;
  uint32 w_VelUncHoriz;
  uint32 w_VelUncVert;
  
  uint32        q_Hdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Pdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Vdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */  
  uint8      u_UtcHour;       /**< Range [0..23] hours */
  uint8      u_UtcMin;        /**< Range [0..59] minutes */
  uint16     w_UtcSec;        /**< Count in units of 10 msec. Range [0..5999] */
  uint32     q_UtcDate;       /* UTC Date ddmmyyyy 
                                 dd  in range of [1..31]
                                 mm in range of [1..12]
                                 yyyy in range of [0..9999]   
                                 */
  uint32  q_LocUncHorizontal;    /*  The HEPE value in meters */
  uint8  u_TimeMask;             /**< BIT[0] = GPS - if set to 1, indicates GPS SVs were used 
                                                     in obtaining the time stamp.
                                      BIT[1] = GLO - if set to 1, indicates GLO SVs were used 
                                                     in obtaining the time stamp.
                                      BIT[2] = GLO Offset Valid - if set to 1, indicates that 
                                                     the Glonass-GPS time offset field is valid.
                                      BIT[3] = BDS - if set to 1, indicates BDS SVs were used 
                                                     in obtaining the time stamp.
                                      BIT[4] = GAL - if set to 1, indicates GAL SVs were used 
                                                     in obtaining the time stamp.
                                 */
  uint32 q_GloOffset;            /**< milliseconds offset between gps and glonass time. */
  uint32 q_Reserved[2];
LOG_RECORD_END

typedef PACKED union PACKED_POST {
  uint32 data;
  char url[255];
} pa_event_data_u_type;

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C)
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint8 u_EventType; 
  uint8 u_Length;
  uint8 u_Reserved;  
  uint8 u_ParameterValue[255];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_NOTIFY_VERIFY_REQUEST_C)
  uint8  u_PktVersion;
  uint32 q_ClientId;
  uint8  u_NotifyType;
  uint16  q_NotifyLength;
  sm_log_nofity_verify_data z_NotifyData;
  uint32 q_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C)
  uint32 q_ClientId;
  uint8 u_CmdType;
  pdsm_pd_cmd_err_e_type z_CmdErrType;
  uint8 u_PacketVersion;
  uint8 u_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C)
  uint32 q_ClientId;
  uint8 u_CmdType;
  pdsm_pa_cmd_err_e_type z_CmdErrType;
  uint8 u_PacketVersion;
  uint8 u_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C)
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  int32 l_Latitude;
  int32 l_Longitude;
  uint32 q_TimeStamp;
  uint8 u_LocUncAng;
  uint8 u_LocUncA;
  uint8 u_LocUncP;
  uint8 u_OptFieldMask;
  int16 x_Altitude;
  uint16 w_Heading;
  uint16 w_VelocityHor;
  uint8 u_FixType;
  int8 s_VelocityVer;
  uint8 u_LocUncertaintyV;
  uint8 u_PositionMethod;
  uint8 u_PositionSource;
  uint8 u_EndSessionCause;
  uint8 u_UncEllConfidence;
  int8 s_utcOffset;
  int32 q_ExtLoclat;
  int32 q_ExtLoclon;
  uint16 w_ExtLocUncA;
  uint16 w_ExtLocUncP;
  int32 q_VelHoriz;
  int32 q_VelVert;
  uint32 q_TimeStampTowMs;
  uint16 w_TimeStampGpsWeek;
  uint8 u_PosReportedToNetwork;
  uint32 w_VelUncHoriz;
  uint32 w_VelUncVert;
  uint32 q_FixTimeRemainingMSec;    /* Time remaining for the Fix to Timeout */
  uint8  u_PosFromOem; 
  uint32        q_Hdop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Pdop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Vdop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */  
  uint8      u_UtcHour;
  uint8      u_UtcMin;
  uint16     w_UtcSec;
  uint32     q_UtcDate;               /* UTC Date ddmmyyyy 
                                         dd  in range of [1..31]
                                         mm in range of [1..12]
                                         yyyy in range of [0..9999]   
                                     */
  uint32  q_LocUncHorizontal;           /*  The HEPE value in meters */
  uint16  w_XtraStartGpsWeek; /*  Current XTRA info is good starting this GPS week */
  uint16  w_XtraStartGpsMinutes; /*  Current XTRA information is good starting this many minutes into the week */
  uint16  w_XtravalidDurationHours; /*  XTRA info is valid for this many hours */

  uint32 q_SvIdGps;             /* sv id of Gps satellites used in the fix */
  uint32 q_SvIdGlo;             /* sv id of Glo satellites used in the fix */
  uint64 t_SvIdBds;             /* sv id of Bds satellites used in the fix */
  uint64 t_SvIdGal;             /* sv id of Gal satellites used in the fix */
  uint8  u_SvIdQzss;            /* sv id of QZSS satellites used in the fix */
  uint32 q_PositionSourceEx; 
  uint8  u_Reserved[8];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C)
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32 q_NmeaSentenceType;
  uint8 u_NmeaSentenceLength;
  uint8 u_NmeaData[200];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_EXT_STATUS_MEAS_REPORT_C)
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint8 u_NumSvs;
  
  uint32 q_EphSvmask;      /* sv mask for the ephemeris */
  uint32 q_AlmSvmask;      /* sv mask for almanac */
  uint32 q_Ionovalid; /* IONO is Valid */
  uint32 q_HealthSvmask; /* SV mask for the health. if the bit is set, health for that SV is good */
  uint32  q_VisibleSvmask;
  uint32  q_Timeuncms; /* Time uncertainty in milli seconds, floor at 20 ms */
  uint32  q_Reserved[4];
  sm_ext_stat_meas_report_data z_MeasData[PDSM_EXT_MEAS_NUM_SVS];
LOG_RECORD_END



typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32  u_Reserved[4];
} LOG_CGPS_PDSM_GPS_STATE_INFO_C_type;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  int32 q_Lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
  int32 q_Lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
  int16 q_Altitude; /* Altitude in meters */
  uint16 w_LocUncVertical; /* vertical unc in meters  */
  uint32 q_Punc; /* Position Unc in meters */
  uint32 q_TimestampTowMs; /* Timestamp in GPS TOW milliseconds */
  uint16 w_TimestampGpsWeek;  /*GPS week number */
  uint8  b_EngineState; /*  GPS engine state on/off */
  uint32 q_TuncMs; /* Time uncertainty in milli seconds */
  uint32 q_EphSvMask; /* SV Mask for the ephemeris */
  uint32 q_AlmSvMask; /* SV Mask for the Almanac */
  uint32 q_IonoValid;  /*  IONO validity */
  uint32 q_HealthMask; /* SV Mask for the Health */
  uint32 q_VisibleSvMask; /* SV Mask of visible SVs */
  uint16 w_XtraStartGpsWeek; /*  Current XTRA info is good starting this GPS week */
  uint16 w_XtraStartGpsMinutes; /*  Current XTRA information is good starting this many minutes into the week */
  uint16  w_XtraValidDurationHours; /*  XTRA info is valid for this many hours */
  uint32  q_Reserved[4];
} LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32  q_Reserved[4];
} LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32 sessionStatus;
  /**<   Session status.
  
        Valid values: \n
          - 0x00000000 -- SESS_STATUS_SUCCESS \n
          - 0x00000001 -- SESS_STATUS_IN_PROGRESS \n
          - 0x00000002 -- SESS_STATUS_GENERAL_FAILURE \n
          - 0x00000003 -- SESS_STATUS_TIMEOUT \n
          - 0x00000004 -- SESS_STATUS_USER_END \n
          - 0x00000005 -- SESS_STATUS_BAD_PARAMETER \n
          - 0x00000006 -- SESS_STATUS_PHONE_OFFLINE \n
          - 0x00000007 -- SESS_STATUS_ENGINE_LOCKED
       */
  uint8 sessionId;
  /**<    ID of the session that was specified in the Start request 
        LOC_ENG_START_REQ. \n
        - Type: Unsigned integer \n
        - Range: 0 to 255  */
  /*****  The position report information  which is reported to the QMI client ****/
  /*  Latitude */
  uint8 latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude; 
  /**<   Latitude (specified in WGS84 datum). \n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -90.0 to 90.0 \n
       - Positive values indicate northern latitude \n
       - Negative values indicate southern latitude  */

  /*   Longitude */
  uint8 longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -180.0 to 180.0 \n
       - Positive values indicate eastern longitude \n
       - Negative values indicate western longitude  */

  /*   Circular Horizontal Position Uncertainty */
  uint8 horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Type: Floating point \n
       - Units: Meters  */

  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  uint8 horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  uint8 horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /*  Elliptical Horizontal Uncertainty Azimuth */
  uint8 horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Type: Floating point \n
       - Units: Decimal degrees \n
       - Range: 0 to 180  */

  /*  Horizontal Circular Confidence */
  uint8 horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8 horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99  */

  /*  Horizontal Reliability */
  uint8 horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  uint32 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \n
         - 0x00000000 -- RELIABILITY_NOT_SET \n
         - 0x00000001 -- RELIABILITY_VERY_LOW \n
         - 0x00000002 -- RELIABILITY_LOW \n
         - 0x00000003 -- RELIABILITY_MEDIUM \n
         - 0x00000004 -- RELIABILITY_HIGH
    */

  /*  Horizontal Speed */
  uint8 horSpeed_valid;  /**< Must be set to true if horSpeed is being passed */
  float horSpeed;
  /**<   Horizontal speed.\n
       - Type: Floating point \n 
       - Units: Meters/second  */

  /*  Horizontal Speed Uncertainty */
  uint8 horSpeedUnc_valid;  /**< Must be set to true if horSpeedUnc is being passed */
  float horSpeedUnc;
  /**<   2D (horizontal) Speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second  */

  /*  Altitude With Respect to Ellipsoid */
  uint8 altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Type: Floating point \n 
       - Units: Meters \n
       - Range: -500 to 15883  */

  /*  Altitude With Respect to Sea Level */
  uint8 altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Type: Floating point \n 
       - Units: Meters  */

  /*  Vertical Uncertainty */
  uint8 vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters  */

  /*  Vertical Confidence */
  uint8 vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8 vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99  */

  /*  Vertical Reliability */
  uint8 vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  uint32 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \n
          - 0x00000000 -- RELIABILITY_NOT_SET \n
          - 0x00000001 -- RELIABILITY_VERY_LOW \n
          - 0x00000002 -- RELIABILITY_LOW \n
          - 0x00000003 -- RELIABILITY_MEDIUM \n
          - 0x00000004 -- RELIABILITY_HIGH  */

  /*  Vertical Speed */
  uint8 vertSpeed_valid;  /**< Must be set to true if vertSpeed is being passed */
  float vertSpeed;
  /**<   Vertical speed.\n
         - Type: Floating point \n
         - Units: Meters/second  */

  /*  Vertical Speed Uncertainty */
  uint8 vertSpeedUnc_valid;  /**< Must be set to true if vertSpeedUnc is being passed */
  float vertSpeedUnc;
  /**<   1-D (vertical) speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second  */

  /*  Heading */
  uint8 heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading.\n
         - Type: Floating point \n
         - Units: Degrees \n
         - Range: 0 to 359.999   */

  /*  Heading Uncertainty */
  uint8 headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty.\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: 0 to 359.999  */

  /*  Magnetic Deviation */
  uint8 magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown 
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. \n
      - Type: Floating point  */

  /*  Technology Used */
  uint8 technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  uint32 technologyMask;
  /**<   Technology used in computing this fix.

       Valid bitmasks: \n
         - 0x00000001 -- SATELLITE \n
         - 0x00000002 -- CELLID \n
         - 0x00000004 -- WIFI \n
         - 0x00000008 -- SENSORS \n 
         - 0x00000010 -- REFERENCE_LOCATION  */

  /*  Dilution of Precision */
  uint8 PDOP_valid;  /**< Must be set to true if PDOP is being passed */
  float PDOP;
  /**<   Position dilution of precision.\n
       - Type: Floating point \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)\n
       - PDOP = square root of (HDOP^2 + VDOP^2)        */

  uint8 HDOP_valid;  /**< Must be set to true if HDOP is being passed */
  float HDOP;
  /**<   Horizontal dilution of precision.\n
       - Type: Floating point \n  
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)  */

  uint8 VDOP_valid;  /**< Must be set to true if VDOP is being passed */
  float VDOP;
  /**<   Vertical dilution of precision.\n
       - Type: Floating point. \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)  */

  /*  UTC Timestamp */
  uint8 timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64 timestampUtc;
  /**<   UTC timestamp, including the leap seconds \n
       - Type: Unsigned integer \n 
       - Units: Milliseconds since Jan. 1, 1970  */

  uint8 leapSeconds_valid;
  uint8 leapSeconds;

  /*  Time Uncertainty */
  uint8 timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Type: Floating point \n
       - Units: Milliseconds   */

  /*  Time Source */
  uint8 timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  uint32 timeSrc;
  /**<   Time source.
  
        Valid values: \n
          - 0x00000000 -- TIME_SRC_INVALID \n 
          - 0x00000001 -- TIME_SRC_NETWORK_TIME_TRANSFER \n  
          - 0x00000002 -- TIME_SRC_NETWORK_TIME_TAGGING \n   
          - 0x00000003 -- TIME_SRC_EXTERNAL_ INPUT \n         
          - 0x00000004 -- TIME_SRC_TOW_DECODE \n             
          - 0x00000005 -- TIME_SRC_TOW_CONFIRMED \n          
          - 0x00000006 -- TIME_SRC_TOW_AND_WEEK_CONFIRMED \n 
          - 0x00000007 -- TIME_SRC_NAV_SOLUTION \n  
          - 0x00000008 -- TIME_SRC_SOLVE_FOR_TIME  */

  /*  Sensor Data Usage */
  uint8 sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  uint32 sensorUsageMask;
  /**<   Specifies which sensors are used.
  
       Valid bitmasks are specified by the following constants: \n       
         - 0x00000001 -- SENSOR_USED_ACCEL \n
         - 0x00000002 -- SENSOR_USED_GYRO  */

  uint32 sesorAidingIndicatorMask;
  /**<   Specifies which results are aided by sensors.

       Valid bitmasks are specified by the following constants: \n
         - 0x00000001 -- AIDED_HEADING \n
         - 0x00000002 -- AIDED_SPEED \n
         - 0x00000004 -- AIDED_POSITION \n
         - 0x00000008 -- AIDED_VELOCITY  */

  /*  SV's Used to Calculate the Fix */
  uint8 gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32 gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16 gnssSvUsedList[LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01];

  /*  Horizontal Elliptical Confidence */
  uint8 horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8 horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99  */

  /*  GPS Time */
  uint8 gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  uint16 gpsWeek;
  /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
       - Type: Unsigned integer \n
       - Units: Weeks  */

  uint32 gpsTimeOfWeekMs;
  /**<   Amount of time into the current GPS week. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */

  /**<   \n The number of weeks since Jan. 5, 1980, and
       milliseconds into the current week.  */


  /* End of position report information */
  uint32  q_Reserved[4];
} LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32 q_flags; /* flags*/
  int32 q_Lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
  int32 q_Lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
  uint32 q_Punc; /* Position Unc in meters */
  uint32 q_TimestampTowMs; /* Timestamp in GPS TOW milliseconds */
  uint16 w_TimestampGpsWeek;  /*GPS week number */
} LOG_CGPS_PDSM_EXT_STATUS_POS_INJ_REQ_INFO_C_type;

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

LOG_RECORD_DEFINE(LOG_GNSS_SM_ERROR_C)
   uint8    u_PacketVersion;
   uint8    u_Severity;
   uint32   q_Reserved[2];
   uint8    u_Length;
   uint8    u_MessageData[1];
LOG_RECORD_END

/* OEMDRE DM log structures */

/* Measurement Report specific logs*/

typedef PACKED struct PACKED_POST
{
  uint32 f_SubMs; /* Range of 0 thru 0.99999 [msecs] */
  uint32 q_Ms; /* GPS: Range of 0 thru (WEEK_MSECS-1) [msecs] */
  /*  GLO: Range of 0 thru (DAY_MSECS-1)  [msecs] */
} gnss_SvTime_DmLog;

typedef PACKED struct PACKED_POST
{
  gnss_SvTime_DmLog z_SvTime;            /* SV Time information */
  uint32      f_SvTimeUncMs;       /* Time uncertainty (msec) */
  uint32      f_SvSpeed;           /* Speed estimate (meters/sec) */
  uint32      f_SvSpeedUnc;        /* Speed uncertainty estimate (meters/sec) */
} gnss_TimeSpeedStructType_ForDmLog;

typedef PACKED struct PACKED_POST
{
  uint16  w_Sv;                /* GPS/BDS/GAL SVPRN or GLONASS SV Slot Id */
  int8    b_FreqNum;           /* GLONASS frequency number + 7 (1 - 14)  */
  uint32  e_SvState;           /* SV Observation state */
  uint8   u_Observe;           /* Count of all observation (both success and failure) */
  uint8   u_GoodObs;           /* Count of Good observations */
  uint8   u_FilterN;           /* Carrier to Code filtering N count */
  uint8   u_PreInt;            /* Pre-Detection (Coherent) Interval (msecs) */
  uint8   u_CSlipCount;        /* Increments when a CSlip is detected */
  uint16  w_PostInt;           /* Num Post-Detections (uints of PreInts) */
  uint32  q_MeasStatus;        /* Bit mask indicating measurement status */
  uint32  q_MeasStatus2;       /* 2nd bit mask indicating measurement status */
  uint16  w_Cno;               /* CNo. Units of 0.1 dB */
  uint16  w_RfLoss;            /* GLO Rf loss reference to Antenaa, units of .1 dB */
  int16   x_LatencyMs;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */

  gnss_TimeSpeedStructType_ForDmLog z_FiltMeas;    /* Filtered measurement */
  gnss_TimeSpeedStructType_ForDmLog z_UnfiltMeas;  /* Un-filtered measurement */

  boolean u_MultipathEstValid; /* TRUE is Multipath estimate is valid */
  uint32  f_MultipathEst;      /* Multipath estimate (meters) */
  boolean u_SvDirValid;        /* SV direction validity flag */
  uint32  f_Azimuth;           /* Azimuth (radians) */
  uint32  f_Elevation;         /* Elevation (radians) */
  uint32  f_DopplerAccel;      /* Acceleration in Hz/Sec */
  uint32  f_FineSpeed;         /* Carrier phase derived speed */
  uint32  f_FineSpeedUnc;      /* Carrier phase derived speed UNC */
  uint64  d_CarrierPhase;      /* Carrier phase measurement [L1 cycles] */
  uint32  q_FCount;
  uint16  w_ParityErrorCount;  /* The 10 LSB to indicate parity errors in the 10 words of current subframe
                                  1: error  0: no error
                                  bit0 for word 1, …, bit9 for word 10  */
  uint8   u_GoodParity;        /* Provides indication whether any parity errors have been observed in 
                                  the last second for each GPS SV.  1:Good Parity 0:Bad Parity*/

} gnss_MeasStructType_ForDmLog;

typedef PACKED struct PACKED_POST
{
  uint32  f_ClkFreqBias;  /* Clock frequency bias. [m/s] */
  uint32  f_ClkFreqUnc; /* 1 sided max freq bias uncertainty [m/s] */
  uint8   e_SourceOfFreq;
} gnss_FreqStructType_DmLog;

typedef PACKED struct PACKED_POST
{
  uint8                 e_SourceOfTime; /* Source of the time */
  uint8                 u_FourYear;     /* GLO 4 year number from 1996 at the reference tick*/
  uint16                w_Days;         /* GLO day number in 4 years at the reference tick*/
  uint32                q_GloMsec;      /* GLO msec in the day at reference tick [msecs] */
  uint32                f_ClkTimeBias;  /* Clock bias [msecs] */
  uint32                f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */
} glo_TimeStructType_DmLog;

typedef PACKED struct PACKED_POST
{
  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  uint32 f_ClkTimeBias; /* Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
  uint32 f_ClkTimeUncMs;/* 1 sided max time bias uncertainty [msecs] */
  uint8  e_SourceOfTime; /* Source of the time */
} gps_TimeStructType_DmLog;

typedef PACKED struct PACKED_POST
{
  uint8                     u_DeltaLeapSec;    /* GPS time leap second delta to UTC time */
  uint8                     u_DeltaLeapSecUnc; /* Uncertainty for GPS leap second */
  uint32                    f_GGTBMs;    /* GPS to Glonass Time Bias */
  uint32                    f_GGTBUncMs; /* Uncertainty for GPS to Glonass Time Bias */
  gps_TimeStructType_DmLog  z_GnssTime; /* GPS|BDS|GAL Time at w_RefFCount */
  glo_TimeStructType_DmLog  z_GloTime; /*GLO Time at the reference counter */
} gnss_TimeStructType_DmLog;

typedef PACKED struct PACKED_POST
{
  boolean                      u_SystemRtcValid;
  uint32                       q_RefFCount;      /* FCount value at reference tick */
  uint32                       q_NumClockResets; /* Cumulative # of Clock resets. */
  uint64                       t_SystemRtcMs;    /* System RTC value at clock validity */
  gnss_TimeStructType_DmLog    z_Time;   /* Time at q_RefFCount */
  gnss_FreqStructType_DmLog    z_Freq;   /* Freq at q_RefFCount */
} gnss_ClockStructType_DmLog;

typedef PACKED struct PACKED_POST
{
  /* CDMA RTC value latched at the last Sync80 tick (in CDMA Chipx8) */
  uint32  q_CdmaRtcSync80CCx8;
  /* GPS RTC value at Sync80 */
  uint32  q_GpsRtcSubMsSync80GCx32;
  /* CDMA RTC value latched at the same time as GPS RTC (in CDMA Chipx32) */
  uint32  q_CdmaRtcLatchedCCx32;
  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  uint32  q_GpsRtcLatchedGCx32;
  uint32 q_FCount;
} gps_CDMAClockInfoStructType_ForDmLog;

typedef PACKED union PACKED_POST
{
  gnss_MeasStructType_ForDmLog   z_Gps[N_ACTIVE_GPS_CHAN];
  gnss_MeasStructType_ForDmLog   z_Glo[N_ACTIVE_GLO_CHAN];
  gnss_MeasStructType_ForDmLog   z_Bds[N_ACTIVE_BDS_CHAN];
  gnss_MeasStructType_ForDmLog   z_Gal[N_ACTIVE_GAL_CHAN];
} gnss_Meas;

typedef PACKED struct PACKED_POST
{
  uint8                                  u_NumSvs;             /* Num SV for which measurements are valid */
  uint8                                  u_SeqNum;             /* Sequence Number */
  uint8                                  u_MaxMessageNum;      /* Maximum Number of message */
  uint16                                 w_RFLoss;             /* RF Loss with reference to Antenna. Units of 0.1dB */
  gnss_ClockStructType_DmLog             z_MeasClock;          /* GNSS clock information for the GPS or GLONASS measurements */
  gps_CDMAClockInfoStructType_ForDmLog   z_CDMAClockInfo;      /* CDMA and RTC Values at Time Transfer */
  uint8                                  e_MeasBlkSrc;         /* Source of the measurements in the GNSS measurement block */
  gnss_Meas                              n_Meas;
} gnss_MeasBlkStructType_ForDmLog;


/* Inter-constellation Time Bias data as generated by PE */
typedef PACKED struct PACKED_POST
{
  uint32 f_UnfiltMeters;        /* Raw TB in meters */
  uint32 f_UnfiltUncMeters;     /* Uncertianty of raw TB in meters */
  uint32 q_FiltAgeMs;           /* Msec since last update */
  uint32 f_FiltMeters;          /* Filtered TB in meters */
  uint32 f_FiltUncMeters;       /* Uncertainty of filtered TB in meters */
  boolean b_IsPeTbConfident;    /* TRUE: PE computed TB is confident, FALSE: otherwise. */
} gnss_PeTimeBiasStructType_ForDmLog;

/* Position Report specific logs*/
typedef PACKED struct PACKED_POST
{
  
  uint64 w_NavSlnPositionFlags;      /* gnss_NavSlnFlagsStructType Flags */
  uint64 w_NavSlnPositionFlagsExt;   /* gnss_NavSlnFlagsStructType Flags Extended */
  uint16 w_PositionFlags;            /* Flags as defined above. */
  uint16 w_FixWn;                   /* Week number of fix. */
  uint32 q_FixTime;             /* GPS time in week (milliseconds) of fix */
  uint8  u_GloNumFourYear;      /* Glonass four-year cycle number */
  uint16 w_GloNumDaysInFourYear;/* Glonass calendar day within the current four-year cycle */
  uint32 q_GloTimeMs;           /* Glonass time in day (milliseconds) */
  uint32 q_UtcFixTime;          /* Time adjusted for leap seconds. */
  uint32 q_RefFCount;           /* Receiver's ms counter value. */
  uint8  u_InitGpsTimeSrc;      /* Time source information (pdsm_extended_time_src_e_type) */
    /* GNSS Fix Position Information */
  uint32 q_PosCounter;          /* Position structure counter. */
  uint64 d_PosEcef[3];          /* Earth Centered, Earth Fixed position fix (m). */
  uint64 d_PosLla[3];           /* Lat, Long, Alt. */
  uint64 d_VelEcef[3];          /* ECEF velocity fix (m/s). */
  uint32 f_VelEnu[3];           /* East, North, Up velocity (m/s). */
  uint32 f_HeadingRad;          /* Heading angle, [0,2*pi] (rad). Referenced to North (0 rad) */
  uint32 f_HeadingUncRad;       /* Heading uncertainty (rad) */
  uint32 f_GnssHeadingRad;      /* Gnss Only heading */
  uint32 f_GnssHeadingUncRad;   /* Gnss Only heading uncertainty (rad) */
  uint32 f_PosSigma;            /* Position standard deviation estimate (m).  */
  uint32 f_PosUnc2D[2];         /* Lat, Long uncertainty (m). */
  uint32 f_PosUncVertical;      /* Vertial uncertainty (m).   */
  uint32 f_ErrorEllipse[3];     /* loc_uncrtnty_ang (degree), loc_uncrtnty_a (m), loc_uncrtnty_p (m) */
  uint32 f_VelUnc;              /* Velocity uncertainty (m/s). */
  uint32 f_VuncEastNorthMps[2]; /* East, North uncertianty (m/s) */
  uint32 f_VuncVertMps;         /* Vertical velocity uncertainty (m/s) */
  uint32 f_ClockBias;           /* Receiver's clock bias (m).    */
  uint32 f_ClockBiasUncMs;      /* Receiver's clock uncertainty (m). */
  uint32 f_ClockDriftRate;      /* Receiver's clock drift (m/s). */
  uint32 f_ClockDriftRateUncMps;/* Receiver's clock frequency bias uncertainty (m/s). */
  
  gnss_PeTimeBiasStructType_ForDmLog z_InterGnssTb[GNSS_TB_TOTNUM]; /* Inter-constellation Time Biases */
  uint32 f_FilteredAlt;         /* Filtered altitude (m). */
  uint32 f_FilteredAltUnc;      /* Filtered altitude uncertainty, one-sided (m). */
  uint32 f_UnfilteredAlt;       /* Unfiltered altitude (m) */
  uint32 f_UnfilteredAltUnc;    /* Unfiltered altitude uncertainty, one-sided (m) */
  uint32 f_MagDeviation;        /* Magnetic deviation from true north (degrees). */

  uint32 f_PDOP;                /* Position Dilution of Precision (unitless). */
  uint32 f_HDOP;                /* Horizontal DOP (unitless) */
  uint32 f_VDOP;                /* Vertial DOP (unitless) */

  uint8  u_FirstFix;            /* Set TRUE when we have are first fix. */
  uint8  u_DriftEstimated;      /* Set TRUE when the clock drift rate is estimated from
                                                          * a difference of two clock biases. */
  uint8  u_MagDevGood;          /* TRUE if magnetic devation is good. */
  uint8  u_IonoCorrected;       /* FALSE indicates that not all SV measurements were iono corrected */
  uint8  u_TropoCorrected;      /* FALSE indicates that not all SV measurements were tropo corrected */
  
  uint32 e_HoriRelIndicator;    /* horizontal reliability */
  uint32 e_VertRelIndicator;    /* vertical reliability */

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
} sm_GnssNavSolution_ForDmLog;

typedef PACKED struct PACKED_POST{
  uint32 q_SvInfoMask;  /* See SM_GNSS_SV_INFO_XXX definition bits above.
                         * For XXX_VALID bits, if set, then corresponding data is valid, otherwise not valid */
  uint16 w_SvId;        /* GNSS SV: {GPS|BDS|GAL} PRN or {GLO} SlotID. */
  /* GNSS SV-specific data for SVs used in Position Fix (valid if PDSM_GNSS_SV_INFO_USED_POS_FIX is set) */
  uint32  f_PrResMeters; /* Pseudo-range measurement residuals of GNSS SV */
  uint32  f_PrUncMeters; /* Pseudo-range weights (uncertainties) of GNSS SV */

  /* GNSS SV-specific data for SVs used in Velocity Fix (valid if SM_GNSS_SV_INFO_VEL_FIX_VALID is set) */
  uint32  f_PrrResMps;   /* Pseudo-range rate (doppler) measurement residuals of GNSS SV */
  uint32  f_PrrUncMps;   /* Pseudo-range rate (doppler) uncertainties of GNSS SV */

  union /*ToDo_GAL change IODE to be 16 bit*/
  {
    uint16 w_IODE;      // IODE {GPS|BDS|GAL} 
    uint8  u_GloTb;     // Tb for {GLO} SV 
  }; 

  /* GNSS SV-specific data for SVs detected by searcher (valid if SM_GNSS_SV_INFO_MEAS_VALID is set) */
  int8    b_FreqNum;     /* Frequency number(-7~+6) of the Glonass SV, invalid for GPS/BDS SV
                           valid if PDSM_GNSS_SV_INFO_USED_POS_FIX is set as well*/

  uint8   u_SvFSAvail;   /* Indication for FineSpeed Availability 1:Available 0:NotAvailable */
  uint8   u_SvPrtyGood;  /* Indication for whether any parity errors have been
                         * observed in the last second for each GNSS SV
                         * = 1:Parity Good
                         * = 0:Parity Bad
                         */
  uint16  w_SvPrtyErr;   /* {GPS|BDS} the 10 LSB to indicate parity errors in the 10 words of current subframe
                         *     {GLO} boolean type
                         * (bit0 for word 1, ... bit9 for word 10)
                         * = 1:error
                         * = 0:no error
                         */
} pdsm_GnssSvInfoType_ForDmLog;

typedef PACKED struct PACKED_POST
{
  uint16  w_SensorValidMask; /* Indicated which of the axes have valid data
                                Use SENSOR_BIAS_VALID_xxx macro to find which
                                of the axes have valid data*/
  uint32  f_XaxisValue; /*  X-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
  uint32  f_YaxisValue; /*  Y-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
  uint32  f_ZaxisValue; /*  Z-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/   
}sm_SensorBiasValueStructType_ForDmLog;

typedef PACKED struct PACKED_POST
{
  uint16  w_SeqenceNumber; /* Incremental packet serial number starting from 0, to identify missing packets  */
  sm_SensorBiasValueStructType_ForDmLog z_GyroBias;      /* Contains axis validity mask + x,y,z values for Gyro bias */
  sm_SensorBiasValueStructType_ForDmLog z_AccelBias;     /* Contains axis validity mask + x,y,z values for accel bias */
} sm_SensorParamRptStructType_ForDmLog;

typedef PACKED struct PACKED_POST
{
  uint32 q_RefFCount;        /* Receiver's ms counter value. */
  uint32 f_ErrorEllipse[3];  /* loc_uncrtnty_ang (degree), loc_uncrtnty_a (m), loc_uncrtnty_p (m) */
  uint32 f_FilteredAlt;      /* Filtered altitude (m). */
  uint32 f_FilteredAltUnc;   /* Filtered altitude uncertainty, one-sided (m). */
  uint32 f_UnfilteredAlt;    /* Unfiltered altitude (m). */
  uint32 f_UnfilteredAltUnc; /* Unfiltered altitude uncertainty, one-sided (m).  */
  uint8  u_nSVsDetected;     /* total number of GPS SVs detected by searcher */
  uint8  u_nSVs;             /* Number of SVs used in the position fix. */
  uint8  u_GpsNumSvVelFix;   /* Number of GPS SVs used in Velocity Fix */
  uint32 q_GpsSvMaskUsed;    /* Bitmask for GPS SVs used for calculating position fix */ 
  uint32 q_GpsSvMaskUsable;  /* Bitmask for GPS SVs usable for calculating position fix */
  pdsm_GnssSvInfoType_ForDmLog z_GpsSvInfo[PDSM_N_ACTIVE_GPS_CHAN]; /* SV-specific data for GPS supplemental SV information */
  uint8  u_nGloSVsDetected;  /* total number of GLONASS SVs detected by searcher */
  uint8  u_NumGloSvs;        /* Number of Glonass SVs used in the position fix. */
  uint8  u_GloNumSvVelFix;   /* Number of GLO SVs used in Velocity Fix */
  uint32 q_GloSvMaskUsed;    /* Bitmask for GLONASS SVs used for calculating position fix */
  uint32 q_GloSvMaskUsable;  /* Bitmask for GLONASS SVs useable for calculating position fix */
  pdsm_GnssSvInfoType_ForDmLog z_GloSvInfo[PDSM_N_ACTIVE_GLO_CHAN]; /* SV-specific data for GLO supplemental SV information */
  uint8  u_nBdsSVsDetected;  /* total number of BDS SVs detected by searcher */
  uint8  u_NumBdsSvs;        /* Number of BDS SVs used in the position fix. */
  uint8  u_BdsNumSvVelFix;   /* Number of BDS SVs used in Velocity Fix */
  uint64 t_BdsSvMaskUsed;    /* Bitmask for BDS SVs used for calculating position fix */ 
  uint64 t_BdsSvMaskUsable;  /* Bitmask for BDS SVs usable for calculating position fix */
  pdsm_GnssSvInfoType_ForDmLog z_BdsSvInfo[PDSM_N_ACTIVE_BDS_CHAN]; /* SV-specific data for BDS supplemental SV information */
  uint8  u_nGalSVsDetected;  /* total number of Galileo SVs detected by searcher */
  uint8  u_NumGalSvs;        /* Number of Galileo SVs used in the position fix. */
  uint8  u_GalNumSvVelFix;   /* Number of GAL SVs used in Velocity Fix */
  uint64 t_GalSvMaskUsed;    /* Bitmask for GAL SVs used for calculating position fix */ 
  uint64 t_GalSvMaskUsable;  /* Bitmask for GAL SVs usable for calculating position fix */
  pdsm_GnssSvInfoType_ForDmLog z_GalSvInfo[PDSM_N_ACTIVE_GAL_CHAN]; /* SV-specific data for GAL supplemental SV information */
} sm_GnssNavSolutionExtra_ForDmLog;

/* -- OEMDRE Specific Logs -- */
/* DM Log 0x14DE */
typedef PACKED struct PACKED_POST
{
  log_hdr_type               xx_hdr;  /* Standard DM header */
  uint8                      u_PacketVersion;

  gnss_MeasRptReasonEnumType  e_MeasRptReason;
  gnss_MeasBlkStructType_ForDmLog     z_MeasBlk;
} LOG_GNSS_OEMDRE_MEASUREMENT_REPORT_C_type; //gnss_MeasBlkStructType;

/* DM Log 0x14E0 */
typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8   u_PacketVersion;
  boolean b_nofix;

  // Fields for Fix report
  sm_GnssNavSolution_ForDmLog          z_NavSolution;
  sm_GnssNavSolutionExtra_ForDmLog     z_NavSolutionExtra;
  sm_SensorParamRptStructType_ForDmLog z_OnDeviceSensorParam; /* sensor-related info */

  // Fields for no Fix report
  uint16 w_PositionFlags;         /**< Position flags indicating the type of fix attempted*/
  uint32 q_FailureCode;           /**< Failure code for no fix*/
  uint8  u_GpsTimeIsValid;        /**< GPS time information is valid. */
  uint16 w_FixWn;                 /**< Week number of fix. */
  uint32 q_FixTime;               /**< Milliseconds into week for fix. */
  uint32 q_UtcFixTime;            /**< Time adjusted for leap seconds. */
  uint32 q_RefFCount;             /**< Receiver's ms counter value. */
  uint32 w_PositionFlagsEx;       /**< additional position flags to accomodate newer consetallations*/
  uint32 w_Reserved;              /**< reserved for future use */
} LOG_GNSS_OEMDRE_POSITION_REPORT_C_type; // sm_GnssFixRptStructType+sm_GnssNoFixRptStructType;

/* DM Log 0x14E1 */
typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8   u_PacketVersion;
  uint16  w_Sv;           /* Sv ID */ 
  /* GPS: 1-32, GLO: 65-96, 0: Invalid All others are reserved */
  uint8   b_FreqNum;      /* Freq index, only valid if u_SysInd is GLO */
  uint8   u_SvPolyFlags;  /* Indicates the validity of data components, see those
                                  C_SV_POLY_... defintions above */
  uint16  w_Iode;         /* Ephemeris reference time
                         GPS:Issue of Data Ephemeris used [unitless].
                         GLO: Tb 7-bit, refer to ICD02 
                         GAL: 10-bit iode, NOT SUPPORTED! */
  uint64  d_T0;           /* Reference time for polynominal calculations
                         GPS: Secs in week.
                         GLO: Full secs since Jan/01/96 */
  uint64  pd_xyz0[3];     /* C0X, C0Y, C0Z */
  uint64  pd_xyzN[9];     /* C1X, C2X ... C2Z, C3Z */
  uint32  pf_other[4];    /* C0T, C1T, C2T, C3T */
  uint32  f_PosUnc;       /* SV position uncertainty [m]. */
  uint32  f_IonoDelay;    /* Ionospheric delay at d_T0 [m]. */
  uint32  f_IonoDot;      /* Iono delay rate [m/s].  */
  uint32  f_sbasIonoDelay;/* SBAS Ionospheric delay at d_T0 [m]. */
  uint32  f_sbasIonoDot;  /* SBAS Iono delay rate [m/s].  */
  uint32  f_TropoDelay;   /* Tropospheric delay [m]. */
  uint32  f_Elevation;    /* Elevation [rad] at d_T0 */
  uint32  f_ElevDot;      /* Elevation rate [rad/s] */
  uint32  f_ElevUnc;      /* SV elevation [rad] uncertainty */
  uint64  d_VelCoef[12];  /* Coefficients of velocity poly */
} LOG_GNSS_OEMDRE_SVPOLY_REPORT_C_type; //gnss_SvPoly;

/* DM Log 0x14E2 */
#define LOG_GNSS_OEMDRSYNC_C_PULSE_DESC 0
#define LOG_GNSS_OEMDRSYNC_C_CONFIG     1

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8   u_PacketVersion;

  uint8   u_DRSyncLogType;

  /* Pulse Desc */
  FLT    f_GpsMsec;     /* GPS msec at reference tick [msecs] */
  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  FLT    f_ClkTimeUncMs;/* 1 sided max time bias uncertainty [msecs] */
  uint32 q_Count;       /* Number of pulses generated */

  /* Config */
  gnss_DRSyncModeType e_Mode;         /* DR Sync mode */
  gnss_DRSyncPolarityType e_Polarity; /* Polarity of the sync pulse */
  FLT f_BiasNs;                     /* Bias of the pulse in nsec */
  int32 l_PhaseSec;                 /* The phase in seconds of the pulse 
                                    relative to GPS week */
  int32 l_PeriodSec;                /* The period of the pulse in seconds */
  FLT f_TuncStartThresholdMs;       /* The time uncertainty below which 
                                    the pulse generation will start */
  FLT f_TuncStopThresholdMs;        /* The time uncertainty above which 
                                    the pulse generation will stop */
  boolean b_TuncIgnore;             /* Flag to ignore TUNC */
  boolean b_Armed;                  /* Flag to arm the next pulse */
} LOG_GNSS_OEMDRSYNC_C_type;


typedef PACKED struct PACKED_POST
{
   /* log header.  DO NOT ACCESS DIRECTLY! */
   log_hdr_type hdr;

   /* Version of this packet.  Currently only 0 is supported */
   uint8    u_PacketVersion;

   /* Eph Download Data */
   sm_log_eph_download_data     z_EphDownloadData;

} LOG_CGPS_PDSM_EPH_DOWNLOAD_C_type;


/* LOG_GNSS_FAST_TCAL_structure */
#define LOG_GNSS_FAST_TCAL_VERSION  1

typedef PACKED struct PACKED_POST
{
  /* log header.  DO NOT ACCESS DIRECTLY! */
  log_hdr_type hdr;
  
  uint8    q_Version;

  uint8    u_ErrorCode; /* == 0: no error; everything else: error */

  float    f_TCalAverValueNsec;       /* Average TCal Value in nsec */
  uint32   q_TCalUncertaintyNsec;     /* Calculated TCal uncertainty value in nsec */
  float    f_TCalMaxValueNsec;        /* Max TCal Value in nsec */
  float    f_TCalMinValueNsec;        /* Min TCal Value in nsec */

  float    f_PpmAverToaCCx16;         /* Average Time of arrival in CDMA Chipx16 */
  int32    l_PpmSrchWinCenterCCx16;   /* PPM 1x SRCH window center in CDMA Chipx16 */
  uint32   q_PpmNumToaSamplesAccumulated;  /* No of PPM samples */
  int16    x_PpmMaxToaCCx16;          /* Max Time of arrival in CDMA Chipx16 */
  int16    x_PpmMinToaCCx16;          /* Min Time of arrival in CDMA Chipx16 */
  float    f_PpmAverEcIodB;           /* Average RSSI of PPM in dB */
  float    f_PpmMaxEcIodB;            /* Max RSSI reported in PPM in dB */
  float    f_PpmMinEcIodB;            /* Min RSSI reported in PPM in dB */

  float    f_TtGnssRtcS80SubMsecGCx80; /* GNSS RTC time @ Sync 80 in GNSS Chipx80 */
  float    f_TtRefCdmaRtcS80SubPnRollCCx16;  /* Ref 1x RTC time @ Sync 80 CDMA Chipx16 */
  float    f_TtRefCdmaRtcS80AdjustSubPnRollCCx16; /* Adjustment chips to be added to align 
                                                    with CDMA RTC @ sync80 in CDMA Chipx16*/
  uint32   q_TtRefGnssRtcs80FCount;     /* TT ref GNSS RTC @ Sync 80 FCount */
  uint32   q_TtNumTTSamplesAccumulated;  /* No of Time Transfer samples accumulated */
  float    f_TtMaxGnssRtcS80SubMsecGCx80; /* Max TT GNSS RTC @ Sync 80 Finger Cnt */
  float    f_TtMinGnssRtcS80SubMsecGCx80; /* Min TT GNSS RTC @ Sync 80 Finger Cnt */

  uint8    u_GpsSv;                       /* GPS SV id */
  float    f_GpsPeakCodePhSubMsec;        /* GPS Code Phase in Sub msec */
  uint32   q_GpsPeakCodePhMsec;           /* GPS Code Phase in msec */
  uint32   q_GpsPeakFCount;               /* GPS Peak FCount in msec */
  float    f_GpsPeakCNodBHz;              /* GPS Peak C/No in dBHz */
  uint32   q_GpsPeakLatency;              /* GPS Peak latency in msec */
  float    f_GpsSpeedMperSec;             /* GPS Doppler in meters/sec  */
  uint32   q_GpsParams; /* (bit 0) == 1: 15ppm apply, (bit 0) == 0: no freq corrections */
}
LOG_GNSS_FAST_TCAL_C_type;

typedef LOG_GNSS_FAST_TCAL_C_type log_ftcal_type;


/*
 ******************************************************************************
 * sm_log_svr_txrx_rep
 *
 * Function description:
 *
 * This function sends a DM log message for each server message either sent
 * or received.  If there is no memory space available or the logging for this
 * DM message has not been enabled, then the log_alloc will return a NULL ptr.
 *
 * If the message length is beyond a defined limit, we only send the message
 * in multiple segments each of the defined limit length.  The flags indicate 
 * whether each segment is the first, middle, or last.  Total message length
 * info is provided in the first segment.  Offset into the message is provided 
 * for non-first segments.
 *
 * The function copies the pointers arguments, so arguments are left 
 * unaffected upon return.
 *
 * Parameters: 
 *
 *  u_TxRx - indicates whether a server tx or rx operation is taking place
 * u_Link - indicates the link type over which this communication is taking 
 *          place
 * u_Protocol - indicates the protocol, so downstream apps can decode 
 *              appropriately 
 * w_Len - Amount of bytes involved in the tx or rx operation.
 * p_Dat - pointer to beginning of tx or rx data
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_svr_txrx_rep( uint8 u_TxRx, uint8 u_Link, uint8 u_Protocol, 
                  uint16 w_Len, uint8 *p_Dat )
{
  uint16 w_DataPos, w_SendLen;
  uint16 w_LogLength, w_LengthRemain;
  uint8    u_FirstSegment = C_LSM_LOG_REP_COMM_FLAG_FIRST_SEGMENT;
  uint8    u_MoreSegments;
  static uint8 u_LsmLogTxSeqNum = 0;
  static uint8 u_LsmLogRxSeqNum = 0;
  uint8* p_B = p_Dat; /* copy so pointer is unaffected upon function return */

  LOG_CGPS_REP_SVR_TX_LOG_PACKET_C_type*  log_ptr_tx;
  LOG_CGPS_REP_SVR_RX_LOG_PACKET_C_type*  log_ptr_rx;

  if (p_B == NULL)
  {
    return;
  }

  /* Increment seq num separately for Tx and Rx.  Increment in all cases, so
   * we can detect any packets not sent due to insufficient memory, or packets
   * not received by app.  */

  if( u_TxRx == C_LSM_LOG_SVR_TX_TYPE )
    w_DataPos = FPOS( LOG_CGPS_REP_SVR_TX_LOG_PACKET_C_type, u_Data );
  else if ( u_TxRx == C_LSM_LOG_SVR_RX_TYPE )
    w_DataPos = FPOS( LOG_CGPS_REP_SVR_RX_LOG_PACKET_C_type, u_Data );
  else 
    return; /* not tx or rx; invalid so exit */


  w_LengthRemain = w_Len;
  while ( w_LengthRemain > 0 )
  {
    /* Check if there will be more segments after this one */
    if( w_LengthRemain > C_LSM_LOG_SVR_REP_MAX_DATA_LEN ) 
    {
      u_MoreSegments = C_LSM_LOG_REP_COMM_FLAG_MORE_SEGMENTS;
      w_SendLen = C_LSM_LOG_SVR_REP_MAX_DATA_LEN;
    }
    else
    {
      u_MoreSegments = 0;
      w_SendLen = w_LengthRemain;
    }
    w_LogLength = w_DataPos + w_SendLen;

    /* Need separate log_alloc to obtain different log ID for Tx & Rx. 
     * If not Tx or Rx, the above check already returned and exited. */

    if( u_TxRx == C_LSM_LOG_SVR_TX_TYPE )
    {
      u_LsmLogTxSeqNum++; /* increment always for every Tx log attempt */
      log_ptr_tx = 
        (LOG_CGPS_REP_SVR_TX_LOG_PACKET_C_type*)log_alloc( LOG_CGPS_REP_SVR_TX_LOG_PACKET_C, w_LogLength );

      if ( log_ptr_tx != 0 )  /* sufficient space and log is enabled */
      {
        /* fill the structure */

        log_ptr_tx->u_Link    = u_Link;
        log_ptr_tx->u_Protocol  = u_Protocol;
        log_ptr_tx->u_SeqNum    = u_LsmLogTxSeqNum;
        log_ptr_tx->u_Flags   = u_MoreSegments | u_FirstSegment;

        if( u_FirstSegment )
          /* Send total length in first segment */
          log_ptr_tx->w_Count = w_Len;
        else
          /* Send current offset for start of this segment, for 
           * non-first segments; a zero value represents the 
           * beginning of the message */
          log_ptr_tx->w_Count = w_Len - w_LengthRemain; 
        
        /* disabling lint buffer over-run warning 
           because u_data is defined as char[1] */
/*lint -e{669} reason described above */
        memscpy((void*)log_ptr_tx->u_Data, w_SendLen, p_B, w_SendLen ); 

        log_commit( log_ptr_tx );
      }

      p_B += w_SendLen;
    }
    else 
    {
      u_LsmLogRxSeqNum++; /* increment always for every Rx log attempt */
      log_ptr_rx = 
        (LOG_CGPS_REP_SVR_RX_LOG_PACKET_C_type*)log_alloc( 
              LOG_CGPS_REP_SVR_RX_LOG_PACKET_C, w_LogLength );

      if ( log_ptr_rx != 0 )  /* sufficient space and log is enabled */
      {
        /* fill the structure */

        log_ptr_rx->u_Link    = u_Link;
        log_ptr_rx->u_Protocol  = u_Protocol;
        log_ptr_rx->u_SeqNum    = u_LsmLogRxSeqNum;
        log_ptr_rx->u_Flags   = u_MoreSegments | u_FirstSegment;

        if( u_FirstSegment )
          /* Send total length in first segment */
          log_ptr_rx->w_Count = w_Len;
        else
          /* Send current offset for start of this segment, for 
           * non-first segments; a zero value represents the 
           * beginning of the message */
          log_ptr_rx->w_Count = w_Len - w_LengthRemain; 

        /* disabling lint buffer over-run warning 
           because u_data is defined as char[1] */
/*lint -e{669} reason described above */
        memscpy((void*)log_ptr_rx->u_Data, w_SendLen, p_B, w_SendLen );

        log_commit( log_ptr_rx );
      }

      p_B += w_SendLen;
    }

    /* Subtract amount just handled; result can't be negative based 
     * on logic above. */
    w_LengthRemain -= w_SendLen;

    u_FirstSegment = 0; /* first segment done */

  } /* while ( w_LengthRemain > 0 ) */

}




/*
 ******************************************************************************
 * sm_log_sm_event
 *
 * Function description:
 *
 * This function sends a DM log message with LSM specific event information.  
 * This is sent out asynchronously when the event occurs.
 *
 * If there is no memory space available or the logging for this
 * DM message has not been enabled, then the log_alloc will return a NULL ptr.
 *
 *
 * Parameters: 
 *
 *  p_EvtInfo - pointer to structure containing the event information
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_sm_event( sm_report_evt_type *p_EvtInfo )
{
  LOG_CGPS_REP_EVT_LOG_PACKET_C_type*  p_LogPkt;

  if (p_EvtInfo == NULL) 
  {
    return;
  }

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_REP_EVT_LOG_PACKET_C_type*)log_alloc(
              LOG_CGPS_REP_EVT_LOG_PACKET_C,
                       sizeof( LOG_CGPS_REP_EVT_LOG_PACKET_C_type ) );

  if ( p_LogPkt != 0 ) /* sufficient space and log is enabled */
  {
    p_LogPkt->u_EventId   = p_EvtInfo->u_EventId;
    p_LogPkt->u_EventData = p_EvtInfo->u_EventData;
    p_LogPkt->u_PacketVersion = p_EvtInfo->u_PacketVersion;
    p_LogPkt->u_Reserved2 = p_EvtInfo->u_Reserved2;

    log_commit( p_LogPkt );

  }

}


/*
 ******************************************************************************
 * sm_log_event_report
 *
 * Function description:
 *
 * This function acts as the interface between the event report and the log
 * function which actually sends out the log for the event.  This function
 * assumes only the EventId and EventData is provided, and therefore sets the
 * Reserved bytes to 0.
 *
 *
 * Parameters: 
 *
 * u_EventId - enumerated value for the event which occurred
 *  u_EventData - data associated with the event which occurred
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_event_report( uint8 u_EventId, uint8 u_EventData )
{

  sm_report_evt_type z_event;

  z_event.u_EventId   = u_EventId;
  z_event.u_EventData = u_EventData;
  z_event.u_PacketVersion = 0;
  z_event.u_Reserved2 = 0;

  sm_log_sm_event( &z_event );

}


/*
 ******************************************************************************
 * sm_log_get_position_request
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a pdsm_get_position 
 *  was called.
 *
 *
 * Parameters: 
 *
 *  pdsm_cmd_s_type - data associated with the event which occurred
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_get_position_request(pdsm_cmd_s_type*  p_CmdType)
{
  LOG_CGPS_PDSM_GET_POSITION_C_type*  p_LogPkt;
  uint8 i=0;
  uint8 j=0;

  if (p_CmdType == NULL) 
  {
    return;
  }

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_GET_POSITION_C_type*)log_alloc(
          LOG_CGPS_PDSM_GET_POSITION_C, sizeof(
          LOG_CGPS_PDSM_GET_POSITION_C_type));

  if ( p_LogPkt != 0 ) /* sufficient space and log is enabled */
  {
    p_LogPkt->w_ServiceType = 0;
    p_LogPkt->u_SessionType =
            (uint8) p_CmdType->cmd.pd.info.option_type.session;
    p_LogPkt->u_OperationMode =
            (uint8) p_CmdType->cmd.pd.info.option_type.operation_mode;
    p_LogPkt->u_DatadownloadMode = 0;
    p_LogPkt->w_DatadownloadDuration = 0;
    p_LogPkt->q_NumFixes =
            p_CmdType->cmd.pd.info.option_type.fix_rate.num_fixes;
    p_LogPkt->q_TimebetFixes =
            p_CmdType->cmd.pd.info.option_type.fix_rate.time_between_fixes_ms;
    p_LogPkt->u_PrivacyLevel = 0;
    p_LogPkt->u_ServerOption = 
            (uint8) p_CmdType->cmd.pd.info.option_type.lsinfo.server_option;
    
    p_LogPkt->q_ServerAddress = 0;
    p_LogPkt->q_PortId = 0;
    p_LogPkt->u_GpsSessionTimeout =
            p_CmdType->cmd.pd.info.qos.gps_session_timeout;
    p_LogPkt->q_AccuracyThreshold =
            p_CmdType->cmd.pd.info.qos.accuracy_threshold;
    p_LogPkt->q_ClientId = (uint32) p_CmdType->cmd.pd.client_id;
    for (i=0; i<8; i++)  {
      p_LogPkt->u_Reserved[i] = 0; 
    } 
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->u_ServerAddrType = (uint8)p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_addr_type;
    for (i=0; i<16; i++)
      p_LogPkt->q_ServerAddress_v4v6[i] = 0;
    p_LogPkt->q_ServerPort_v4v6 = 0;
    p_LogPkt->u_UrlLength = 0;
    p_LogPkt->u_ServerURL[0] = '\0';
    if ( p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS)
    {
      p_LogPkt->q_ServerAddress_v4v6[15] =
        (uint8) p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs & 0xFF;
      p_LogPkt->q_ServerAddress_v4v6[14] =
        (uint8) ((p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs & 0xFF00) >> 8);
      p_LogPkt->q_ServerAddress_v4v6[13] =
        (uint8) ((p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs & 0xFF0000) >> 16);
      p_LogPkt->q_ServerAddress_v4v6[12] =
        (uint8) ((p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs & 0xFF000000) >> 24);
      p_LogPkt->q_ServerPort_v4v6 =
         (uint32)p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id; 
      
    } 
    else if( p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_IPV6_ADDRESS)
    {
        for (i=0, j=0; i<15 && j<8; i=i+2, j++) 
        {
          p_LogPkt->q_ServerAddress_v4v6[i] = (uint8) p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v6.server_adrs[j] & 0xff;
          if (i+1 < 16)
          {
            p_LogPkt->q_ServerAddress_v4v6[i+1] =
            (uint8) ((p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v6.server_adrs[j] & 0xff00) >> 8);
          }
        }
      p_LogPkt->q_ServerPort_v4v6 =
         (uint32)p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v6.port_id;
      
    } 
    else if( p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS)
    {
      p_LogPkt->u_UrlLength = (uint8) p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_url.url_length; 
      (void)GNSS_STRLCPY((char*)p_LogPkt->u_ServerURL,
              (char*)p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_url.url,
              p_CmdType->cmd.pd.info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_url.url_length);
      
    }
    p_LogPkt->q_ClassId = (uint32)p_CmdType->cmd.pd.info.option_type.class_id;
    for (i = 0; i<16; i++)
     p_LogPkt->u_Reserved2[i] = 0;
    log_commit( p_LogPkt );

  }
}

/*
 ******************************************************************************
 * sm_log_set_parameters_request
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a pdsm_set_parameters 
 *  was called.
 *
 *
 * Parameters: 
 *
 *  pdsm_cmd_s_type - data associated with the event which occurred
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_set_parameters_request(pdsm_cmd_s_type*  p_CmdType)
{
  LOG_GNSS_PDSM_SET_PARAMETERS_C_type*  p_LogPkt;
  uint8 i, j;
  
  if (p_CmdType == NULL)
  {
    return;
  }
  p_LogPkt = (LOG_GNSS_PDSM_SET_PARAMETERS_C_type*)log_alloc(
               LOG_GNSS_PDSM_SET_PARAMETERS_C, sizeof(
               LOG_GNSS_PDSM_SET_PARAMETERS_C_type) );


  if (p_LogPkt == NULL )
  {
    return;
  }
  p_LogPkt->u_PacketVersion =  CGPS_SM_LOG_PKT_VERSION_3; 
  p_LogPkt->q_ClientId = (uint32)p_CmdType->cmd.pa.client_id;
  p_LogPkt->u_ParameterType = (uint8)p_CmdType->cmd.pa.info.set_type;
  
  p_LogPkt->u_Length = 4;
  switch (p_CmdType->cmd.pa.info.set_type)
  {
    case PDSM_PA_1X_PDE_NETWORK_ADRS:       /* Adress of the network entity, can be IP or URL */
    case PDSM_PA_1X_MPC_NETWORK_ADRS:       /* Adress of the network entity, can be IP or URL */
    case PDSM_PA_UMTS_PDE_NETWORK_ADRS:       /* Adress of the network entity, can be IP or URL */
      if (p_CmdType->cmd.pa.info.pa_set.server_address.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS)
      {
        p_LogPkt->u_Length = (uint8)p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_url.url_length + 1;
        p_LogPkt->u_ParameterValue[0] = 1;
        (void)GNSS_STRLCPY((char*)&p_LogPkt->u_ParameterValue[1],
                (char*)p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_url.url,
                p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_url.url_length);
      } 
      else if (p_CmdType->cmd.pa.info.pa_set.server_address.server_addr_type == PDSM_SERVER_TYPE_IPV6_ADDRESS)
      {
        p_LogPkt->u_Length = 21;
        p_LogPkt->u_ParameterValue[0] = 2;
        for (i=1, j=0; i<17 && j<8; i=i+2, j++) 
        {
          p_LogPkt->u_ParameterValue[i] = (uint8)p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v6.server_adrs[j];
          p_LogPkt->u_ParameterValue[i+1] = (uint8)(p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v6.server_adrs[j]& 0xF0 >> 4);
        }
        p_LogPkt->u_ParameterValue[17] = (uint8)p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v6.port_id  & 0xFF;
        p_LogPkt->u_ParameterValue[18] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v6.port_id & 0xFF00) >> 8);
        p_LogPkt->u_ParameterValue[19] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v6.port_id & 0xFF0000) >> 16);
        p_LogPkt->u_ParameterValue[20] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v6.port_id & 0xFF000000) >> 24);
      }
      else
      {
        p_LogPkt->u_Length = 9;
        p_LogPkt->u_ParameterValue[0] = 0;
        p_LogPkt->u_ParameterValue[1] = (uint8)p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF;
        p_LogPkt->u_ParameterValue[2] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF00) >> 8);
        p_LogPkt->u_ParameterValue[3] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF0000) >> 16);
        p_LogPkt->u_ParameterValue[4] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF000000) >> 24);
        p_LogPkt->u_ParameterValue[5] = (uint8)p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.port_id & 0xFF;
        p_LogPkt->u_ParameterValue[6] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.port_id & 0xFF00) >> 8);
        p_LogPkt->u_ParameterValue[7] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.port_id & 0xFF0000) >> 16);
        p_LogPkt->u_ParameterValue[8] = (uint8)((p_CmdType->cmd.pa.info.pa_set.server_address.server_adrs.server_adrs_v4.port_id & 0xFF000000) >> 24);
      }
      break;
    case PDSM_PA_GPS_LOCK:           /* GPS LOCK */ 
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.gps_lock;
      break;
    case PDSM_PA_KEEP_WARM:          /* Enable/Disable Keep WARM */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.keep_warm_enable;
      break;
    case PDSM_PA_DELETE_PARAMS_EX1: /* intentional cascade */
    case PDSM_PA_DELETE_PARAMS_EX: /* intentional cascade */
    /*lint -fallthrough */ 
    case PDSM_PA_DELETE_PARAMS: /* Delete GPS related parameters from the database */
      { /* create new stack */
         uint32 q_temp = 0; /* temporarily uint32 hold data to be pumped out */
         uint8 *pOutputByteArray = p_LogPkt->u_ParameterValue;
         p_LogPkt->u_Length = SM_LOG_LENGTH_PA_EVENT_DELETE_PARAMS; 

         //lower 32-bits
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.pdsm_delete_parms_flags & CGPS_MAX_UINT32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         //upper 32-bits
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.pdsm_delete_parms_flags>>32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.time_unc;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.pos_unc;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.time_offset ;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.pos_offset;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = (uint32)p_CmdType->cmd.pa.info.pa_set.delete_params.eph_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = (uint32)p_CmdType->cmd.pa.info.pa_set.delete_params.alm_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.glo_eph_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.glo_alm_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.sbas_eph_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.sbas_alm_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         //BDS EPH low
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_eph_sv_mask & CGPS_MAX_UINT32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
          //BDS EPH hi
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_eph_sv_mask>>32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         //BDS ALM low
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_alm_sv_mask & CGPS_MAX_UINT32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         //BDS ALM hi
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_alm_sv_mask>>32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         //GAL EPH low
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_eph_sv_mask & CGPS_MAX_UINT32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         //GAL EPH hi
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_eph_sv_mask>>32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         //GAL ALM low
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_alm_sv_mask & CGPS_MAX_UINT32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         //GAL ALM hi
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_alm_sv_mask>>32);
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.qzss_eph_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.qzss_alm_sv_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.gps_gnss_sv_blacklist_mask;  
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.glo_gnss_sv_blacklist_mask;  
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);  
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_gnss_sv_blacklist_mask>>32);  
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         //GAL BLACKLIST low
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);    
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);
         //GAL BLACKLIST hi
         q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_gnss_sv_blacklist_mask>>32);  
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.qzss_gnss_sv_blacklist_mask;  
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.delete_celldb_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

         q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.delete_clock_info_mask;
         (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
         pOutputByteArray += sizeof(q_temp);

      }
      break;
      
    case PDSM_PA_POSITION_MODE: /* Position modes that are supported on the mobile */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.position_mode;
      break;
    case PDSM_PA_MT_LR_SUPPORT: /* Specifies if MT-LR is supported */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.mt_lr_support;
      break;
    case PDSM_PA_MO_METHOD: /* specifies if a CP or UP protocol is used for MO */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.mo_method;
      break;
    case PDSM_PA_NMEA_SENTENCE_TYPE: /* specifies the NEMA sentence type to be reported to PDSM */
      p_LogPkt->u_Length = SM_LOG_LENGTH_PA_EVENT_NMEA_SENTENCE_TYPE;
      p_LogPkt->u_ParameterValue[0] = SM_LOG_EXTRACT_BYTE(0, p_CmdType->cmd.pa.info.pa_set.nmea_sentence_type);
      p_LogPkt->u_ParameterValue[1] = SM_LOG_EXTRACT_BYTE(1, p_CmdType->cmd.pa.info.pa_set.nmea_sentence_type);
      p_LogPkt->u_ParameterValue[2] = SM_LOG_EXTRACT_BYTE(2, p_CmdType->cmd.pa.info.pa_set.nmea_sentence_type);
      p_LogPkt->u_ParameterValue[3] = SM_LOG_EXTRACT_BYTE(3, p_CmdType->cmd.pa.info.pa_set.nmea_sentence_type);
      break;
    case PDSM_PA_NMEA_CONFIG_INFO: /* specifies the NMEA configuration info */
     p_LogPkt->u_Length = 8;
     p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_port_type & 0xff;
     p_LogPkt->u_ParameterValue[1] = (uint8)((p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_port_type & 0xff00) >> 8);
     p_LogPkt->u_ParameterValue[2] = (uint8)((p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_port_type & 0xff0000) >> 16);
     p_LogPkt->u_ParameterValue[3] = (uint8)((p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_port_type & 0xff000000) >> 24);
     p_LogPkt->u_ParameterValue[4] = (uint8)p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_reporting_type & 0xff;
     p_LogPkt->u_ParameterValue[5] = (uint8)((p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_reporting_type & 0xff00) >> 8);
     p_LogPkt->u_ParameterValue[6] = (uint8)((p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_reporting_type & 0xff0000) >> 16);
     p_LogPkt->u_ParameterValue[7] = (uint8)((p_CmdType->cmd.pa.info.pa_set.nmea_config_info.pd_nmea_reporting_type & 0xff000000) >> 24);
     break;
    case PDSM_PA_SBAS_USER_PREFERENCE:          /* Enable/Disable SBAS user preference */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.sbas_user_preference;
      break;
    case PDSM_PA_DPO_CONTROL: /* DPO Control */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.dpo_control;
      break;
    
    case PDSM_PA_ON_DEMAND_LOW_POWER_MODE:
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.on_demand_lpm;
      break;
    case PDSM_PA_ON_DEMAND_READY_MODE:
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.on_demand_ready;
      break;
    
    case PDSM_PA_CALL_FLOW_SELECT:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.call_flow_select_mask;
      break;

    case PDSM_PA_EXTERNAL_SPI:
     {
        uint32 q_temp = 0; /* temporarily hold uint32 data to be pumped out */
        uint8 *pOutputByteArray = p_LogPkt->u_ParameterValue;

        /* size of xspi log data. Change if output contains more than just the mode */
        p_LogPkt->u_Length = sizeof(p_CmdType->cmd.pa.info.pa_set.external_spi.mode); 

        q_temp = p_CmdType->cmd.pa.info.pa_set.external_spi.mode;
        (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        /* handle other xspi fields here */
     }
     break;

     case PDSM_PA_LPP_CONFIG_INFO:
       p_LogPkt->u_Length = 1;
       p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.lpp_config_info;
       break;

     case PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG:
       {
          uint32 q_temp = 0; /* temporarily hold uint32 data to be pumped out */
          uint8 *pOutputByteArray = p_LogPkt->u_ParameterValue;
          /* size of gnss_emergency_protocol_select log data. */
          p_LogPkt->u_Length = sizeof(p_CmdType->cmd.pa.info.pa_set.gnss_emergency_protocol_select); 
          q_temp = p_CmdType->cmd.pa.info.pa_set.gnss_emergency_protocol_select;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
       }
       break;


      case PDSM_PA_WIFI_SCAN_TIMEOUT_SEC:
        p_LogPkt->u_Length = 1;
        p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.wifi_wait_timeout_sec;
        break;

    default:
      break;
  }   

  p_LogPkt->u_Reserved[0] = 0;
  p_LogPkt->u_Reserved[1] = 0;
  p_LogPkt->u_Reserved[2] = 0;
  log_commit( p_LogPkt );

}
/*
 ******************************************************************************
 * sm_log_end_session
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a pdsm_end_session 
 *  was called.
 *
 *
 * Parameters: 
 *
 *  session_type - end session type
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_end_session(pdsm_client_id_type          l_ClientId,
                         pdsm_pd_end_session_e_type  e_SessionType)
{
  LOG_CGPS_PDSM_END_SESSION_C_type*  p_LogPkt;
  uint8 i=0;

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_END_SESSION_C_type*)log_alloc(
          LOG_CGPS_PDSM_END_SESSION_C, 
          sizeof(LOG_CGPS_PDSM_END_SESSION_C_type) );

  if ( p_LogPkt != 0 ) /* sufficient space and log is enabled */ {
    p_LogPkt->q_ClientId = (uint32) l_ClientId;
    p_LogPkt->u_SessionType = (uint8) e_SessionType;
    
    for (i=0; i<3; i++)  {
      p_LogPkt->u_Reserved[i] = 0; 
    } 
    log_commit( p_LogPkt );
  }

}

/*
 ******************************************************************************
 * sm_log_notify_verify_response
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a 
 * pdsm_notify_verify_response was called.
 *
 * Parameters: 
 *   pdapi_ver_resp_lcs_ptr: pointer to the lcs structure of the pdapi 
 *     command notif_verification_response
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_notify_verify_response(pdsm_lcs_cmd_s_type *p_LcsType)
{
  LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C_type*  p_LogPkt;
  uint16 w_DataPos;
  uint8 i=0;
  
  if (p_LcsType == NULL)
  {
    return;
  }

  p_LogPkt = (LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C_type*)log_alloc(
      LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C, 
      sizeof(LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C_type));

  if (p_LogPkt == NULL) 
  {
    return;
  }
  p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->q_ClientId = (uint32) p_LcsType->client_id;
  
  
  switch (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type)
  {
    case PDSM_LCS_CMD_TYPE_VX_REQ:
      /* Try to allocate memory for the log packet */
      w_DataPos = sizeof(sm_log_1x_notify_verify_data);

      p_LogPkt->u_NotifyStatus = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.resp;
      p_LogPkt->u_NotifyType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type;
      p_LogPkt->q_NotifyLength = w_DataPos;
      p_LogPkt->z_NotifyData.oneX_data.u_NotificationPrivType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.notification_priv_type;
      p_LogPkt->z_NotifyData.oneX_data.u_PosTechInd = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_tech;
      p_LogPkt->z_NotifyData.oneX_data.u_PosQoSPres = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_qos_incl;
      p_LogPkt->z_NotifyData.oneX_data.u_PosQoS = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_qos;
      p_LogPkt->z_NotifyData.oneX_data.q_NumFixes = (uint32)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.num_fixes;
      p_LogPkt->z_NotifyData.oneX_data.q_Tbf = (uint32)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.tbf;
      p_LogPkt->z_NotifyData.oneX_data.u_PosMode = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_mode;
      p_LogPkt->z_NotifyData.oneX_data.u_DatacodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.encoding_scheme;
      p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.requester_id.requester_id_length;
      if(p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen != 0) 
        for (i=0; i<p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen; i++)
          p_LogPkt->z_NotifyData.oneX_data.u_RequestorId[i] = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.requester_id.requester_id[i];
      p_LogPkt->z_NotifyData.oneX_data.w_UserRespTimer = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.user_resp_timer_val;
      break;
    case PDSM_LCS_CMD_TYPE_UMTS_CP_REQ:
      w_DataPos = sizeof(sm_log_umts_notify_verify_data);
      p_LogPkt->u_NotifyStatus = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.resp;
      p_LogPkt->u_NotifyType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type;
      p_LogPkt->q_NotifyLength = w_DataPos;
      
      p_LogPkt->z_NotifyData.umts_data.u_NotificationPrivType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.notification_priv_type;
      p_LogPkt->z_NotifyData.umts_data.u_SessionInvokeId = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.invoke_id;
      p_LogPkt->z_NotifyData.umts_data.w_Flags = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags;
      p_LogPkt->z_NotifyData.umts_data.u_NotificationTextLength = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.notification_length;
      memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_NotificationText), sizeof(p_LogPkt->z_NotifyData.umts_data.u_NotificationText),
	  	       p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.notification_text,
             p_LogPkt->z_NotifyData.umts_data.u_NotificationTextLength );
      
      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LCS_CLIENT_EXTADDR_PRESENT) 
      {
        p_LogPkt->z_NotifyData.umts_data.u_DatacodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.datacoding_scheme;
        p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddressLen = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.ext_client_address_data.ext_client_address_len;
        memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddress), sizeof(p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddress),
			     p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.ext_client_address_data.ext_client_address,
                p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddressLen);
      } 
      p_LogPkt->z_NotifyData.umts_data.u_LocationType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.location_type;

      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LCS_DEF_LOCATION_TYPE_PRESENT) 
      {
        p_LogPkt->z_NotifyData.umts_data.u_DeferredLocationUnusedBits = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.deferred_location.unused_bits;
        p_LogPkt->z_NotifyData.umts_data.u_DeferredLocationMsAvail = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.deferred_location.ms_available;
      }
      
      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LCS_REQUESTOR_ID_PRESENT) 
      {
        p_LogPkt->z_NotifyData.umts_data.u_RequestorIDDataCodeingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.requestor_id.data_coding_scheme;
        p_LogPkt->z_NotifyData.umts_data.u_RequestorIdLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.requestor_id.string_len;
        memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_RequesterID), sizeof(p_LogPkt->z_NotifyData.umts_data.u_RequesterID),
			     p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.requestor_id.requestor_id_string,
                p_LogPkt->z_NotifyData.umts_data.u_RequestorIdLen);
      } 
      
      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LCS_CODEWORD_PRESENT) 
      {
        p_LogPkt->z_NotifyData.umts_data.u_CodewordDataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.codeword_string.data_coding_scheme;
        p_LogPkt->z_NotifyData.umts_data.u_CodewordLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.codeword_string.string_len;
        memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_CodewordStr), sizeof(p_LogPkt->z_NotifyData.umts_data.u_CodewordStr),
			    p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.codeword_string.lcs_codeword_string,
                p_LogPkt->z_NotifyData.umts_data.u_CodewordLen);
      } 
      
      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LCS_SERVICE_TYPE_ID_PRESENT) 
        p_LogPkt->z_NotifyData.umts_data.u_ServiceTypeId = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.service_type_id.lcs_service_type_id;
 
 
      break;
    case PDSM_LCS_CMD_TYPE_SUPL_REQ:
      /* Try to allocate memory for the log packet */
      w_DataPos = sizeof(sm_log_supl_notify_verify_data);
      p_LogPkt->u_NotifyStatus = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.resp;
      p_LogPkt->u_NotifyType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type;
      p_LogPkt->q_NotifyLength = w_DataPos;
      p_LogPkt->z_NotifyData.supl_data.u_NotificationPrivType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.notification_priv_type;
      p_LogPkt->z_NotifyData.supl_data.w_Flags = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.flags   ;
      p_LogPkt->z_NotifyData.supl_data.u_DataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.datacoding_scheme;
      p_LogPkt->z_NotifyData.supl_data.u_posMethod = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.pos_method;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[0] = 0;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[1] = 0;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[2] = 0;
      for (i=0; i< 8; i++)
        p_LogPkt->z_NotifyData.supl_data.u_SlpHash[i] = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_hash[i];

      p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.requestor_id.string_len;
      if (p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen != 0)
      {
        for (i=0; i< p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen; i++)
          p_LogPkt->z_NotifyData.supl_data.u_RequestorId[i] = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.requestor_id.requestor_id_string[i];
        p_LogPkt->z_NotifyData.supl_data.u_RequstorIdDataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.requestor_id.data_coding_scheme;

      }
      p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.client_name.string_len;
      if (p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen != 0)
      {
        for(i=0; i< p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen; i++)
          p_LogPkt->z_NotifyData.supl_data.u_ClientName[i] = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.client_name.client_name_string[i];
        p_LogPkt->z_NotifyData.supl_data.u_ClientNameDataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.client_name.data_coding_scheme;

      }
 
      p_LogPkt->z_NotifyData.supl_data.w_UserRespTimer = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.user_response_timer;
      p_LogPkt->z_NotifyData.supl_data.u_QopFlags = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.bit_mask;
      if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_VALID ) 
      {
        p_LogPkt->z_NotifyData.supl_data.u_QopHorAcc = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.horacc;
        if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_VERACC_VALID ) 
          p_LogPkt->z_NotifyData.supl_data.u_QopVerAcc = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.veracc;
        if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_MAXAGE_VALID ) 
          p_LogPkt->z_NotifyData.supl_data.w_MaxLocAge = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.maxLocAge;
        if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_DELAY_VALID ) 
          p_LogPkt->z_NotifyData.supl_data.u_Delay = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.delay;
      }
      p_LogPkt->z_NotifyData.supl_data.u_SlpSessIdPres = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.presence;
      for (i=0; i<4; i++)
        p_LogPkt->z_NotifyData.supl_data.u_SlpSessId[i] = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.session_id[i];
      p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.choice;
      if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType  == PDSM_SERVER_TYPE_IPV4_ADDRESS ) // 0: ipv4
      {
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[0] = (uint8)(p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[1] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF00) >> 8);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[2] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF0000) >> 16);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[3] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF000000) >> 24);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[4] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF) );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[5] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF00) >> 8 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[6] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF0000) >> 16 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[7] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF000000) >> 24);
      }
      
      else if ( p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType == PDSM_SERVER_TYPE_URL_ADDRESS ) // 1: FQDN
      {
        (void)GNSS_STRLCPY((char*)&p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[0],
                          (char*)(p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url),
                          p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length+1);
      }
      
      else if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType == PDSM_SERVER_TYPE_IPV6_ADDRESS) // 2: ipv6
      {
        for (i=0; i<8; i=i+2) 
        {
          p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[i] = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[i] ;
          p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[i+1] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[i] & 0xF0) >> 4);
        }
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[16] = (uint8)(p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF) ;
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[17] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF00) >> 8 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[18] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF0000) >> 16 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[19] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF000000) >> 24);
      }
      
      break;
      
    default:
    w_DataPos = 0;
    break;
  }
  p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->q_ClientId = (uint32) p_LcsType->client_id;
  p_LogPkt->q_Reserved[0] = 0;
  p_LogPkt->q_Reserved[1] = 0;
  log_commit( p_LogPkt );
}


/*
 ******************************************************************************
 * sm_log_report_position
 *
 * Function description:
 *
 * This function generates a DM log message when position report is recvd by the
 * PDAPI
 *
 * Parameters: 
 * pointer to sm_pd_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_report_position(const pdsm_pd_info_s_type *p_PdInfoType, boolean sft, boolean kf)
{

  LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C_type *p_LogPkt;
  uint8 i=0;
  pdsm_pd_ext_pos_data_s_type p_ExtPosData;
  
  if (p_PdInfoType == NULL) 
  {
    return;
  }

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C_type*)log_alloc( LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C,
      sizeof(LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = GNSS_GEN8_LOG_VERSION; /* new Callisto log */
    /* p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3; */

    p_LogPkt->q_ClientId = (uint32)p_PdInfoType->client_id;
    p_LogPkt->l_Longitude = p_PdInfoType->pd_info.pd_data.lon;
    p_LogPkt->l_Latitude = p_PdInfoType->pd_info.pd_data.lat;
    p_LogPkt->x_Altitude = p_PdInfoType->pd_info.pd_data.altitude -500; // subtract 500 before logging
    p_LogPkt->q_TimeStamp = p_PdInfoType->pd_info.pd_data.time_stamp;
    p_LogPkt->u_LocUncAng = p_PdInfoType->pd_info.pd_data.loc_uncertainty_ang;
    p_LogPkt->u_LocUncA  = p_PdInfoType->pd_info.pd_data.loc_uncertainty_a;
    p_LogPkt->u_LocUncP = p_PdInfoType->pd_info.pd_data.loc_uncertainty_p;
    p_LogPkt->u_OptFieldMask = p_PdInfoType->pd_info.pd_data.opt_field_mask;
    p_LogPkt->w_Heading = p_PdInfoType->pd_info.pd_data.heading;
    p_LogPkt->w_VelocityHor = p_PdInfoType->pd_info.pd_data.velocity_hor;
    p_LogPkt->u_FixType = p_PdInfoType->pd_info.pd_data.fix_type;
    p_LogPkt->s_VelocityVer = p_PdInfoType->pd_info.pd_data.velocity_ver;
    p_LogPkt->u_LocUncertaintyV = p_PdInfoType->pd_info.pd_data.loc_uncertainty_v;
    p_LogPkt->s_utcOffset = p_PdInfoType->pd_info.pd_data.gpsUtcOffset;
    p_LogPkt->u_UncEllConfidence = p_PdInfoType->pd_info.pd_data.loc_uncertainty_conf;

    switch (p_PdInfoType->pd_info.pd_data.position_mode)
    {
        case PDSM_PD_POSITION_MODE_MSBASED:
          p_LogPkt->u_PositionMethod = 0;
          break;

        case PDSM_PD_POSITION_MODE_MSASSISTED:
          p_LogPkt->u_PositionMethod = 1;
          break;

        case PDSM_PD_POSITION_MODE_STANDALONE:
          p_LogPkt->u_PositionMethod = 2;
          break;

        case PDSM_PD_POSITION_MODE_UNKNOWN:
          p_LogPkt->u_PositionMethod = 3;
          break;

        default:
          p_LogPkt->u_PositionMethod = 0;
          break;
    }


   p_LogPkt->u_PositionSource = 0;
    switch (p_PdInfoType->pd_info.pd_data.position_type)
    {


      case PDSM_PD_POSITION_TYPE_NEW:
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_OLDNEW;
        break;

      default:
        break;

    }

    if ( p_PdInfoType->pd_info.pd_data.position_type != PDSM_PD_POSITION_TYPE_INVALID)
    {
      switch (p_PdInfoType->pd_info.pd_data.positioning_source )
      {
        case PDSM_PD_POSITION_SOURCE_CELLID:
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_CELLID;
          break;

        case PDSM_PD_POSITION_SOURCE_DEFAULT:
          p_LogPkt->u_PositionSource |=  POSITION_SOURCE_DEFAULT;
          break;

        case PDSM_PD_POSITION_SOURCE_UNKNOWN:
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_UNKNOWN;
          break;

        default:
           /* above are mutually exclusive. however, in the following cases, either
           GNSS system might have been used */
           if(PDSM_PD_POSITION_SOURCE_GPS & p_PdInfoType->pd_info.pd_data.positioning_source)
           {
              p_LogPkt->u_PositionSource |= POSITION_SOURCE_GPS;
              if (sft == TRUE)
              {
                 p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
              }
           }

           if(PDSM_PD_POSITION_SOURCE_GLO & p_PdInfoType->pd_info.pd_data.positioning_source)
           {
              p_LogPkt->u_PositionSource |= POSITION_SOURCE_GLO;
              if (sft == TRUE)
              {
                 p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
              }
           }

           /* //RH TODO  u_PositionSource is only uint8
           if (PDSM_PD_POSITION_SOURCE_BDS & p_PdInfoType->pd_info.pd_data.positioning_source)
           {
             p_LogPkt->u_PositionSource |= POSITION_SOURCE_BDS;
             if (sft == TRUE)
             {
               p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
             }
           }
           */

           /* //JV TODO  u_PositionSource is only uint8
           if (PDSM_PD_POSITION_SOURCE_GAL & p_PdInfoType->pd_info.pd_data.positioning_source)
           {
             p_LogPkt->u_PositionSource |= POSITION_SOURCE_GAL;
             if (sft == TRUE)
             {
               p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
             }
           }
           */

          break;
      }
    }
    if (kf == TRUE)
    {
      p_LogPkt->u_PositionSource |= POSITION_SOURCE_KF_WLS;
    }

    p_ExtPosData = p_PdInfoType->pd_info.pd_data.ext_pos_data;
    
    p_LogPkt->q_ExtLoclat = p_ExtPosData.lat;
    p_LogPkt->q_ExtLoclon = p_ExtPosData.lon;
    p_LogPkt->w_ExtLocUncA =  (uint16)p_ExtPosData.alongAxisUnc;
    p_LogPkt->w_ExtLocUncP = (uint16)p_ExtPosData.perpAxisUnc;
    p_LogPkt->q_VelHoriz = (int32)p_ExtPosData.velHor;
    p_LogPkt->q_VelVert  = (int32)p_ExtPosData.velVert;
    p_LogPkt->q_TimeStampTowMs = (uint32)p_ExtPosData.timestamp_tow_ms;
    p_LogPkt->w_TimeStampGpsWeek = (uint16)p_ExtPosData.timestamp_gps_week;
    p_LogPkt->u_PosReportedToNetwork = (uint8)p_ExtPosData.pos_reported_to_network;
    p_LogPkt->w_VelUncHoriz = (uint32)p_ExtPosData.velUncHoriz;
    p_LogPkt->w_VelUncVert = (uint32)p_ExtPosData.velUncVert;
    p_LogPkt->q_Hdop = (uint32)p_ExtPosData.h_dop;               
    p_LogPkt->q_Pdop = (uint32)p_ExtPosData.p_dop;              
    p_LogPkt->q_Vdop = (uint32)p_ExtPosData.v_dop;               
    p_LogPkt->u_UtcHour = p_ExtPosData.utc_time.hour; 
    p_LogPkt->u_UtcMin = p_ExtPosData.utc_time.minute; 
    p_LogPkt->w_UtcSec = p_ExtPosData.utc_time.sec; 
    p_LogPkt->q_UtcDate = p_ExtPosData.utc_time.date; 
    p_LogPkt->q_LocUncHorizontal = p_ExtPosData.loc_unc_horizontal;           

   p_LogPkt->u_TimeMask = 0; 

    if(p_PdInfoType->pd_info.pd_data.ext_pos_data.time_src & PDSM_GNSS_TIME_GPS)
    {
       p_LogPkt->u_TimeMask |= SM_LOG_TIME_MASK_SOURCE_GPS;
    }

    if(p_PdInfoType->pd_info.pd_data.ext_pos_data.time_src & PDSM_GNSS_TIME_GLO)
    {
       p_LogPkt->u_TimeMask |= SM_LOG_TIME_SOURCE_MASK_GLO;
    }

    if(p_PdInfoType->pd_info.pd_data.ext_pos_data.glo_offset_valid)
    {
       p_LogPkt->u_TimeMask |= SM_LOG_TIME_MASK_GLO_GPS_TIMEOFFSET_VALID;
    }
    p_LogPkt->q_GloOffset = p_PdInfoType->pd_info.pd_data.ext_pos_data.glo_offset;

    if(p_PdInfoType->pd_info.pd_data.ext_pos_data.time_src & PDSM_GNSS_TIME_BDS)
    {
       p_LogPkt->u_TimeMask |= SM_LOG_TIME_SOURCE_MASK_BDS;
    }

    if(p_PdInfoType->pd_info.pd_data.ext_pos_data.time_src & PDSM_GNSS_TIME_GAL)
    {
       p_LogPkt->u_TimeMask |= SM_LOG_TIME_SOURCE_MASK_GAL;
    }


    for (i=0; i< GNSS_ARRAY_SIZEOF(p_LogPkt->q_Reserved); i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
}

/*
 ******************************************************************************
 * sm_log_pd_event_callback
 *
 * Function description:
 *
 * This function generates a DM log message when a PD_Event_Callback other than
 * a position report is recvd PDAPI
 *
 * Parameters: 
 * pdsm_pd_event_type
 * pointer to pdsm_pd_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pd_event_callback(pdsm_pd_event_type e_PdEvent, 
    const pdsm_pd_info_s_type *p_PdInfoType)
{

  LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type *p_LogPkt;
  uint8 i;

  if (p_PdInfoType == NULL) 
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type*)log_alloc( 
             LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C,
             sizeof(LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type) );

  if (p_LogPkt == NULL) 
    return;
  p_LogPkt->q_ClientId = (uint32) p_PdInfoType->client_id;
  p_LogPkt->u_EventData = 0;
  switch (e_PdEvent)
  {
    case PDSM_PD_EVENT_END:
      p_LogPkt->u_EventType = PD_EVENT_END;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;
    case PDSM_PD_EVENT_BEGIN:
      p_LogPkt->u_EventType = PD_EVENT_BEGIN;
      break;
    case PDSM_PD_EVENT_DONE:
      p_LogPkt->u_EventType = PD_EVENT_DONE;
      break;
    case PDSM_PD_EVENT_COMM_BEGIN:
      p_LogPkt->u_EventType = PD_EVENT_COMM_BEGIN;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.comm_data.protocol_type;
      break;

    case PDSM_PD_EVENT_COMM_CONNECTED:
      p_LogPkt->u_EventType = PD_EVENT_COMM;
      break;
    case PDSM_PD_EVENT_COMM_FAILURE:
      p_LogPkt->u_EventType =  PD_EVENT_COMM_FAILURE;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;
    case PDSM_PD_EVENT_COMM_DONE:
      p_LogPkt->u_EventType =  PD_EVENT_COMM_DONE;
      break;

    case PDSM_PD_EVENT_WARM_PROC_BEGIN:
      p_LogPkt->u_EventType = PD_EVENT_WARM_PROC_BEGIN;
      break;
    case PDSM_PD_EVENT_WARM:
      p_LogPkt->u_EventType = PD_EVENT_WARM;
      break;
    case PDSM_PD_EVENT_NOT_WARM:
      p_LogPkt->u_EventType = PD_EVENT_NOT_WARM;
      break;
    case PDSM_PD_EVENT_WARM_PROC_DONE:
      p_LogPkt->u_EventType = PD_EVENT_WARM_PROC_DONE;
      break;

    case PDSM_PD_EVENT_GPS_BEGIN:
      p_LogPkt->u_EventType =  PD_EVENT_GPS_BEGIN;
      break;
    case PDSM_PD_EVENT_GPS_DONE:
      p_LogPkt->u_EventType =  PD_EVENT_GPS_END;
      break;

    case PDSM_PD_EVENT_UPDATE_FAILURE:
      p_LogPkt->u_EventType =  PD_EVENT_UPDATE_FAILURE;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;

    case PDSM_PD_EVENT_DLOAD_BEGIN:
      p_LogPkt->u_EventType =  PD_EVENT_DLOAD_BEGIN;
      break;

    case PDSM_PD_EVENT_DLOAD:
      p_LogPkt->u_EventType =  PD_EVENT_DLOAD;
      break;

    case PDSM_PD_EVENT_DLOAD_DONE:
      p_LogPkt->u_EventType =  PD_EVENT_DLOAD_DONE;
      break;

    case PDSM_PD_DLOAD_EVENT_END:
      p_LogPkt->u_EventType =  PD_DLOAD_EVENT_END;
      break;

    /* note PD_DLOAD_INVALID = 19 has no counterpart in pdsm.h,
       therefore it is not currently used */ 

    case PDSM_PD_EVENT_COARSE_POS_INJ_DONE:  
      p_LogPkt->u_EventType = PD_EVENT_COARSE_POS_INJ_DONE;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;

    case PDSM_PD_EVENT_COARSE_POS_INJ_FAILED:
      p_LogPkt->u_EventType =  PD_EVENT_COARSE_POS_INJ_FAILED;
      break;

    case PDSM_PD_EVENT_COARSE_POS_INJ_ERROR:
      p_LogPkt->u_EventType =  PD_EVENT_COARSE_POS_INJ_ERROR;
      break;

    case PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION:
       p_LogPkt->u_EventType =  PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION;
       p_LogPkt->u_EventData = (uint8) p_PdInfoType->pd_info.xspi_data.q_stream_status;
       break;

    case PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION:
       p_LogPkt->u_EventType =  PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION;
       break;

    default:
      p_LogPkt->u_EventType = PD_EVENT_UNDEF_FOR_LOG;
      p_LogPkt->u_EventData = 0;
      break;
  }
  p_LogPkt->u_PacketVersion = GNSS_GEN8_LOG_VERSION; /* new log for Gen8 */
  for (i=0; i<7; i++)
    p_LogPkt->u_Reserved[i] =0;
  log_commit(p_LogPkt);


}

/*
 ******************************************************************************
 * sm_log_pa_event_callback
 *
 * Function description:
 *
 * This function generates a DM log message when a PA_Event_Callback 
 * is recvd PDAPI
 *
 * Parameters: 
 * pdsm_pa_event_type
 * pointer to pdsm_pa_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/


void sm_log_pa_event_callback(pdsm_pa_event_type e_PaEvent, 
    const pdsm_pa_info_s_type *p_PaInfoType)
{

  LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C_type *p_LogPkt;
  uint8 i;
  uint8 j;
  
  if (p_PaInfoType == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet */

  p_LogPkt = (LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C_type*)log_alloc(LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C, 
                                                                sizeof(LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C_type));


  if (p_LogPkt == NULL)
  {
    return;
  }

  p_LogPkt->q_ClientId = (uint32) p_PaInfoType->client_id;
  p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_3; 
  p_LogPkt->u_EventType = (uint8) PDSM_PA_NONE;       /* Init */
  p_LogPkt->u_Length = 4;

  switch (e_PaEvent)
  {
    case PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS:
    /*lint -fallthrough */
    case PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS:
    /*lint -fallthrough */
    case PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS:
      if (e_PaEvent == PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS) 
      {
        p_LogPkt->u_EventType = (uint8)PDSM_PA_1X_PDE_NETWORK_ADRS;
      } 
      else if(e_PaEvent == PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS) 
      {
        p_LogPkt->u_EventType = (uint8)PDSM_PA_1X_MPC_NETWORK_ADRS;
      } 
      else 
      {
        p_LogPkt->u_EventType = (uint8)PDSM_PA_UMTS_PDE_NETWORK_ADRS;
      }
      // fall through
      if (p_PaInfoType->pa_info.server_address.server_addr_type == PDSM_SERVER_TYPE_IPV6_ADDRESS)
      {
        p_LogPkt->u_Length = 21;
        p_LogPkt->u_ParameterValue[0] = 2;
        for (i=1, j=0; i<17 && j<8; i=i+2, j++) 
        {
          p_LogPkt->u_ParameterValue[i] = (uint8)p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v6.server_adrs[j];
          p_LogPkt->u_ParameterValue[i+1] = (uint8)(p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v6.server_adrs[j] & 0xF0 >> 4);
        }
        p_LogPkt->u_ParameterValue[17] = (uint8)p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v6.port_id & 0xFF;
        p_LogPkt->u_ParameterValue[18] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v6.port_id & 0xFF00) >> 8);
        p_LogPkt->u_ParameterValue[19] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v6.port_id & 0xFF0000) >> 16);
        p_LogPkt->u_ParameterValue[20] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v6.port_id & 0xFF000000) >> 24);
      }
      if (p_PaInfoType->pa_info.server_address.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS)
      {
        p_LogPkt->u_Length = 9;
        p_LogPkt->u_ParameterValue[0] = 0;
        p_LogPkt->u_ParameterValue[1] = (uint8)p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF;
        p_LogPkt->u_ParameterValue[2] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF00) >> 8);
        p_LogPkt->u_ParameterValue[3] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF0000) >> 16);
        p_LogPkt->u_ParameterValue[4] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.server_adrs & 0xFF000000) >> 24);
        p_LogPkt->u_ParameterValue[5] = (uint8)p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.port_id & 0xFF;
        p_LogPkt->u_ParameterValue[6] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.port_id & 0xFF00) >> 8);
        p_LogPkt->u_ParameterValue[7] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.port_id & 0xFF0000) >> 16);
        p_LogPkt->u_ParameterValue[8] = (uint8)((p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_v4.port_id & 0xFF000000) >> 24);
      }
      if (p_PaInfoType->pa_info.server_address.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS)
      {
        p_LogPkt->u_Length = (uint8)p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_url.url_length + 2;
        p_LogPkt->u_ParameterValue[0] = 1;
        (void)GNSS_STRLCPY((char*)&p_LogPkt->u_ParameterValue[1],
            (char*)p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_url.url,
            p_PaInfoType->pa_info.server_address.server_adrs.server_adrs_url.url_length);
      }

      break;
    case PDSM_PA_EVENT_GPS_LOCK:
      p_LogPkt->u_Length = 4;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_GPS_LOCK;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.gps_lock ;
      break;
    case PDSM_PA_EVENT_KEEP_WARM:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_KEEP_WARM;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.keep_warm_enable ;
      break;
    case PDSM_PA_EVENT_DELETE_PARAMS:     /* PDSM_PA_DELETE_PARAMS_EX also
                                          uses PDSM_PA_EVENT_DELETE_PARAMS since payloads
                                          are congruent, and there are no free bits
                                          to define a new PA_EVENT */
       { /* create new stack */
          //uint8 u_byte_iter = 0;
          uint32 q_temp = 0;    /* temporarily uint32 hold data to be pumped out */
          uint64 t_pdsm_delete_parms_flags = 0;
          uint8 *pOutputByteArray = p_LogPkt->u_ParameterValue;
        
          p_LogPkt->u_Length = SM_LOG_LENGTH_PA_EVENT_DELETE_PARAMS; 
          p_LogPkt->u_EventType = (uint8)PDSM_PA_DELETE_PARAMS; /* PDSM_PA_DELETE_PARAMS_EX also
                                                                uses this same event since payloads
                                                                are congruent */

          //lower 32-bits
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.pdsm_delete_parms_flags & CGPS_MAX_UINT32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //upper 32-bits
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.pdsm_delete_parms_flags>>32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.time_unc;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.pos_unc;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.time_offset ;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.pos_offset;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.eph_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.alm_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.glo_eph_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.glo_alm_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.sbas_eph_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.sbas_alm_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          //BDS EPH low
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_eph_sv_mask & CGPS_MAX_UINT32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //BDS EPH hi
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_eph_sv_mask>>32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          //BDS ALM low
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_alm_sv_mask & CGPS_MAX_UINT32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //BDS ALM hi
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_alm_sv_mask>>32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          //GAL EPH low
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_eph_sv_mask & CGPS_MAX_UINT32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //GAL EPH hi
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_eph_sv_mask>>32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          //GAL ALM low
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_alm_sv_mask & CGPS_MAX_UINT32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //GAL ALM hi
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_alm_sv_mask>>32);
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.qzss_eph_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.qzss_alm_sv_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.gps_gnss_sv_blacklist_mask;  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.glo_gnss_sv_blacklist_mask;  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_gnss_sv_blacklist_mask>>32);  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //GAL BLACK LIST low
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);
          //GAL BLACK LIST hi
          q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_gnss_sv_blacklist_mask>>32);  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.qzss_gnss_sv_blacklist_mask;  
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.delete_celldb_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

          q_temp = p_PaInfoType->pa_info.delete_params.delete_clock_info_mask;
          (void) sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
          pOutputByteArray += sizeof(q_temp);

       }
      break;
      
    case PDSM_PA_EVENT_POSITION_MODE:
      p_LogPkt->u_Length = 2;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_POSITION_MODE;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.position_mode & 0xff;
      p_LogPkt->u_ParameterValue[1]  = (uint8)((p_PaInfoType->pa_info.position_mode & 0xff00) >> 8);
      break;
    case PDSM_PA_EVENT_MT_LR_SUPPORT:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_MT_LR_SUPPORT;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.mt_lr_support;
      break;
    case PDSM_PA_EVENT_MO_METHOD:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_MO_METHOD;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.mo_method; 
      break;
    case PDSM_PA_EVENT_NMEA_SENTENCE_TYPE:
      p_LogPkt->u_Length = SM_LOG_LENGTH_PA_EVENT_NMEA_SENTENCE_TYPE;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_NMEA_SENTENCE_TYPE;
      p_LogPkt->u_ParameterValue[0] =  SM_LOG_EXTRACT_BYTE(0, p_PaInfoType->pa_info.nmea_sentence_type);
      p_LogPkt->u_ParameterValue[1] =  SM_LOG_EXTRACT_BYTE(1, p_PaInfoType->pa_info.nmea_sentence_type);
      p_LogPkt->u_ParameterValue[2] =  SM_LOG_EXTRACT_BYTE(2, p_PaInfoType->pa_info.nmea_sentence_type);
      p_LogPkt->u_ParameterValue[3] =  SM_LOG_EXTRACT_BYTE(3, p_PaInfoType->pa_info.nmea_sentence_type);

      break;
      
    case PDSM_PA_EVENT_NMEA_CONFIG_INFO: /* specifies the NMEA configuration info */
       p_LogPkt->u_Length = 8;
       p_LogPkt->u_EventType = (uint8)PDSM_PA_NMEA_CONFIG_INFO;
       p_LogPkt->u_ParameterValue[0] = (uint8)p_PaInfoType->pa_info.nmea_config_info.pd_nmea_port_type & 0xff;
       p_LogPkt->u_ParameterValue[1] = (uint8)((p_PaInfoType->pa_info.nmea_config_info.pd_nmea_port_type & 0xff00) >> 8);
       p_LogPkt->u_ParameterValue[2] = (uint8)((p_PaInfoType->pa_info.nmea_config_info.pd_nmea_port_type & 0xff0000) >> 16);
       p_LogPkt->u_ParameterValue[3] = (uint8)((p_PaInfoType->pa_info.nmea_config_info.pd_nmea_port_type & 0xff000000) >> 24);
       p_LogPkt->u_ParameterValue[4] = (uint8)p_PaInfoType->pa_info.nmea_config_info.pd_nmea_reporting_type & 0xff;
       p_LogPkt->u_ParameterValue[5] = (uint8)((p_PaInfoType->pa_info.nmea_config_info.pd_nmea_reporting_type & 0xff00) >> 8);
       p_LogPkt->u_ParameterValue[6] = (uint8)((p_PaInfoType->pa_info.nmea_config_info.pd_nmea_reporting_type & 0xff0000) >> 16);
       p_LogPkt->u_ParameterValue[7] = (uint8)((p_PaInfoType->pa_info.nmea_config_info.pd_nmea_reporting_type & 0xff000000) >> 24);
       break;
      
    case PDSM_PA_EVENT_SBAS_USER_PREFERENCE:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_SBAS_USER_PREFERENCE;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.sbas_user_preference;
      break;
    case PDSM_PA_EVENT_SBAS_STATUS:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_SBAS_STATUS;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.sbas_status;
      break;

    case PDSM_PA_EVENT_DPO_CONTROL:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_DPO_CONTROL;
      p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.dpo_control;
      break;

    case PDSM_PA_EVENT_ON_DEMAND_LPM:
    p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_ON_DEMAND_LOW_POWER_MODE;
    p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.on_demand_lpm;
    break;
    case PDSM_PA_EVENT_ON_DEMAND_READY_MODE:
    p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_ON_DEMAND_READY_MODE;
    p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.on_demand_ready;
    break;
    
    case PDSM_PA_EVENT_PA_CALL_FLOW_SELECT:
    p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_CALL_FLOW_SELECT;
    p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.call_flow_select_mask;
    break;

  case PDSM_PA_EVENT_EXTERNAL_SPI:
     p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_PA_EXTERNAL_SPI;
     p_LogPkt->u_ParameterValue[0]  = (uint8)p_PaInfoType->pa_info.external_spi.mode;
     break;

    default:
      break;
  }

  /*! (New method)
   *  PA event enum based assignment - Assign PA event enum if no conversion to
   *  PA event bitmap takes place in the above section.
  */ 
  if ( p_LogPkt->u_EventType == (uint8) PDSM_PA_NONE )
  {
    p_LogPkt->u_EventType = (uint8)p_PaInfoType->pa_event;

    // For any PA events that aren't covered in the PA event bitmap to enum conversion
    switch ( p_PaInfoType->pa_event )
    {
    case PDSM_PA_ASST_GLONASS_POS_PROTOCOL:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_ParameterValue[0]  = (uint32)p_PaInfoType->pa_info.asst_glo_protocol;
      break;

    case PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_ParameterValue[0] = (uint8)p_PaInfoType->pa_info.gnss_emergency_protocol_select;
      break;

    case PDSM_PA_WIFI_SCAN_TIMEOUT_SEC:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_ParameterValue[0] = (uint8)p_PaInfoType->pa_info.wifi_wait_timeout_sec;
      break;

      default: /* Unsupported PDSM PA events */
        MSG_MED("No logging support for PDSM PA event (%d)!", p_PaInfoType->pa_event, 0, 0);
        break;
    }
  }

  p_LogPkt->u_Reserved =0;
  log_commit(p_LogPkt);
}

/*
 ******************************************************************************
 * sm_log_notification_verification_request
 *
 * Function description:
 *
 * This function generates a DM log message when a notification verification 
 * request is recvd PDAPI
 *
 * Parameters: 
 * pointer to pdsm_lcs_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_notification_verification_request(
        pdsm_lcs_info_s_type *p_LcsInfoType
        )
{
  LOG_CGPS_PDSM_NOTIFY_VERIFY_REQUEST_C_type *p_LogPkt = NULL;
  uint8 i=0;
  uint16 w_DataPos; 
  
  if (p_LcsInfoType == NULL) 
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_NOTIFY_VERIFY_REQUEST_C_type*)log_alloc(
      LOG_CGPS_PDSM_NOTIFY_VERIFY_REQUEST_C, 
      sizeof(LOG_CGPS_PDSM_NOTIFY_VERIFY_REQUEST_C_type)  );
  
  if (p_LogPkt == NULL)
  {
    return;
  }
  switch (p_LcsInfoType->lcs_cb_cmd_type)
  {
    case PDSM_LCS_CMD_TYPE_VX_REQ:
      w_DataPos = sizeof(sm_log_1x_notify_verify_data); 
      p_LogPkt->u_NotifyType = (uint8)p_LcsInfoType->lcs_cb_cmd_type;
      p_LogPkt->q_NotifyLength =w_DataPos;
      p_LogPkt->z_NotifyData.oneX_data.u_NotificationPrivType = (uint8)p_LcsInfoType->lcs_info.vx_req.notification_priv_type;
      p_LogPkt->z_NotifyData.oneX_data.u_PosTechInd = (uint8)p_LcsInfoType->lcs_info.vx_req.pos_tech;
      p_LogPkt->z_NotifyData.oneX_data.u_PosQoSPres = (uint8)p_LcsInfoType->lcs_info.vx_req.pos_qos_incl;
      p_LogPkt->z_NotifyData.oneX_data.u_PosQoS = (uint8)p_LcsInfoType->lcs_info.vx_req.pos_qos;
      p_LogPkt->z_NotifyData.oneX_data.q_NumFixes = (uint32)p_LcsInfoType->lcs_info.vx_req.num_fixes;
      p_LogPkt->z_NotifyData.oneX_data.q_Tbf = (uint32)p_LcsInfoType->lcs_info.vx_req.tbf;
      p_LogPkt->z_NotifyData.oneX_data.u_PosMode = (uint8)p_LcsInfoType->lcs_info.vx_req.pos_mode;
      p_LogPkt->z_NotifyData.oneX_data.u_DatacodingScheme = (uint8)p_LcsInfoType->lcs_info.vx_req.encoding_scheme;
      p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen = (uint8)p_LcsInfoType->lcs_info.vx_req.requester_id.requester_id_length;
      if(p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen != 0) 
        for (i=0; i<p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen ; i++)
          p_LogPkt->z_NotifyData.oneX_data.u_RequestorId[i] = p_LcsInfoType->lcs_info.vx_req.requester_id.requester_id[i];
      p_LogPkt->z_NotifyData.oneX_data.w_UserRespTimer = (uint16)p_LcsInfoType->lcs_info.vx_req.user_resp_timer_val;
      break;
    case PDSM_LCS_CMD_TYPE_UMTS_CP_REQ:
      w_DataPos = (uint16)sizeof(sm_log_umts_notify_verify_data);
      
       p_LogPkt->u_NotifyType = (uint8)p_LcsInfoType->lcs_cb_cmd_type;
       p_LogPkt->q_NotifyLength =w_DataPos;
       p_LogPkt->z_NotifyData.umts_data.u_NotificationPrivType = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.notification_priv_type;
       p_LogPkt->z_NotifyData.umts_data.u_SessionInvokeId = p_LcsInfoType->lcs_info.umts_cp_req.invoke_id;
       p_LogPkt->z_NotifyData.umts_data.w_Flags = p_LcsInfoType->lcs_info.umts_cp_req.flags;
       p_LogPkt->z_NotifyData.umts_data.u_NotificationTextLength = p_LcsInfoType->lcs_info.umts_cp_req.notification_length;
       memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_NotificationText), sizeof(p_LogPkt->z_NotifyData.umts_data.u_NotificationText),
	   	       p_LcsInfoType->lcs_info.umts_cp_req.notification_text,
              p_LogPkt->z_NotifyData.umts_data.u_NotificationTextLength );
       
       if (p_LcsInfoType->lcs_info.umts_cp_req.flags & PDSM_LCS_CLIENT_EXTADDR_PRESENT) 
       {
         p_LogPkt->z_NotifyData.umts_data.u_DatacodingScheme = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.datacoding_scheme;
         p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddressLen = p_LcsInfoType->lcs_info.umts_cp_req.ext_client_address_data.ext_client_address_len;
         memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddress), sizeof(p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddress),
		 	     p_LcsInfoType->lcs_info.umts_cp_req.ext_client_address_data.ext_client_address,
                 p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddressLen);
       } 
       p_LogPkt->z_NotifyData.umts_data.u_LocationType = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.location_type;
 
       if (p_LcsInfoType->lcs_info.umts_cp_req.flags & PDSM_LCS_DEF_LOCATION_TYPE_PRESENT) 
       {
         p_LogPkt->z_NotifyData.umts_data.u_DeferredLocationUnusedBits = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.deferred_location.unused_bits;
         p_LogPkt->z_NotifyData.umts_data.u_DeferredLocationMsAvail = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.deferred_location.ms_available;
       }
       
       if (p_LcsInfoType->lcs_info.umts_cp_req.flags & PDSM_LCS_REQUESTOR_ID_PRESENT) 
       {
         p_LogPkt->z_NotifyData.umts_data.u_RequestorIDDataCodeingScheme = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.requestor_id.data_coding_scheme;
         p_LogPkt->z_NotifyData.umts_data.u_RequestorIdLen = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.requestor_id.string_len;
         memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_RequesterID), sizeof(p_LogPkt->z_NotifyData.umts_data.u_RequesterID),
		 	     p_LcsInfoType->lcs_info.umts_cp_req.requestor_id.requestor_id_string,
                 p_LogPkt->z_NotifyData.umts_data.u_RequestorIdLen);
       } 
       
       if (p_LcsInfoType->lcs_info.umts_cp_req.flags & PDSM_LCS_CODEWORD_PRESENT) 
       {
         p_LogPkt->z_NotifyData.umts_data.u_CodewordDataCodingScheme = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.codeword_string.data_coding_scheme;
         p_LogPkt->z_NotifyData.umts_data.u_CodewordLen = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.codeword_string.string_len;
         memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_CodewordStr), sizeof(p_LogPkt->z_NotifyData.umts_data.u_CodewordStr),
		 	     p_LcsInfoType->lcs_info.umts_cp_req.codeword_string.lcs_codeword_string,
                 p_LogPkt->z_NotifyData.umts_data.u_CodewordLen);
       } 
       
       if (p_LcsInfoType->lcs_info.umts_cp_req.flags & PDSM_LCS_SERVICE_TYPE_ID_PRESENT) 
         p_LogPkt->z_NotifyData.umts_data.u_ServiceTypeId = (uint8)p_LcsInfoType->lcs_info.umts_cp_req.service_type_id.lcs_service_type_id;
  
  
      break;
    case PDSM_LCS_CMD_TYPE_SUPL_REQ:
      w_DataPos = sizeof(sm_log_supl_notify_verify_data); 
      p_LogPkt->u_NotifyType = (uint8)p_LcsInfoType->lcs_cb_cmd_type;
      p_LogPkt->q_NotifyLength =w_DataPos;
      p_LogPkt->z_NotifyData.supl_data.u_NotificationPrivType = (uint8)p_LcsInfoType->lcs_info.supl_req.notification_priv_type;
      p_LogPkt->z_NotifyData.supl_data.w_Flags = (uint16)p_LcsInfoType->lcs_info.supl_req.flags   ;
      p_LogPkt->z_NotifyData.supl_data.u_posMethod = (uint8)p_LcsInfoType->lcs_info.supl_req.pos_method;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[0] = 0;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[1] = 0;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[2] = 0;
      for (i=0; i< 8; i++)
        p_LogPkt->z_NotifyData.supl_data.u_SlpHash[i] = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_hash[i];

      
      p_LogPkt->z_NotifyData.supl_data.u_DataCodingScheme = (uint8)p_LcsInfoType->lcs_info.supl_req.datacoding_scheme;

      p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen = (uint8)p_LcsInfoType->lcs_info.supl_req.requestor_id.string_len;
      if (p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen != 0)
      {
        for (i=0; i<p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen; i++)
          p_LogPkt->z_NotifyData.supl_data.u_RequestorId[i] = p_LcsInfoType->lcs_info.supl_req.requestor_id.requestor_id_string[i];
        p_LogPkt->z_NotifyData.supl_data.u_RequstorIdDataCodingScheme = (uint8)p_LcsInfoType->lcs_info.supl_req.requestor_id.data_coding_scheme;

      }

      p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen = (uint8)p_LcsInfoType->lcs_info.supl_req.client_name.string_len;
      if (p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen != 0)
      {
        for (i=0; i<p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen; i++)
          p_LogPkt->z_NotifyData.supl_data.u_ClientName[i] = p_LcsInfoType->lcs_info.supl_req.client_name.client_name_string[i];
        p_LogPkt->z_NotifyData.supl_data.u_ClientNameDataCodingScheme = (uint8)p_LcsInfoType->lcs_info.supl_req.client_name.data_coding_scheme;

      }
 
      p_LogPkt->z_NotifyData.supl_data.w_UserRespTimer = (uint16)p_LcsInfoType->lcs_info.supl_req.user_response_timer;
      p_LogPkt->z_NotifyData.supl_data.u_QopFlags = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_qop.bit_mask;
      if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_VALID ) 
      {
        p_LogPkt->z_NotifyData.supl_data.u_QopHorAcc = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_qop.horacc;
        if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_VERACC_VALID ) 
          p_LogPkt->z_NotifyData.supl_data.u_QopVerAcc = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_qop.veracc;
        if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_MAXAGE_VALID ) 
          p_LogPkt->z_NotifyData.supl_data.w_MaxLocAge = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_qop.maxLocAge;
        if ( p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LCS_SUPL_QOP_DELAY_VALID ) 
          p_LogPkt->z_NotifyData.supl_data.u_Delay = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_qop.delay;
      }
      p_LogPkt->z_NotifyData.supl_data.u_SlpSessIdPres = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.presence;
      for (i=0; i<4; i++)
        p_LogPkt->z_NotifyData.supl_data.u_SlpSessId[i] = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.session_id[i];
      p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.choice;
      if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType  == PDSM_SERVER_TYPE_IPV4_ADDRESS ) // 0 : ipv4
      {
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[0] = (uint8)(p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[1] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF00) >> 8);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[2] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF0000) >> 16);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[3] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF000000) >> 24);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[4] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF) );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[5] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF00) >> 8 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[6] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF0000) >> 16 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[7] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF000000) >> 24);
      }
      else if ( p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType == PDSM_SERVER_TYPE_URL_ADDRESS ) // 1: FQDN
      {
        (void)GNSS_STRLCPY((char*)&p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[0],
                          (char*)(p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url),
                          p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length+1);
      }
      
      else if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType == PDSM_SERVER_TYPE_IPV6_ADDRESS ) // 2:  ipv6
      {
        for (i=0; i<8; i=i+2) 
        {
          p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[i] = (uint8)p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[i] ;
          p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[i+1] = (uint8)(p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[i] & 0xF0) >> 4;
        }
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[16] = (uint8)(p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF) ;
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[17] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF00) >> 8 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[18] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF0000) >> 16 );
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[19] = (uint8)((p_LcsInfoType->lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF000000) >> 24);
      }

      break;
    default:
    w_DataPos = 0;
    break;
  }
  p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->q_ClientId = (uint32) p_LcsInfoType->client_id;
  p_LogPkt->q_Reserved[0] = 0;
  p_LogPkt->q_Reserved[1] = 0;
  log_commit( p_LogPkt );

}

/*
 ******************************************************************************
 * sm_log_pd_cmd_err
 *
 * Function description:
 *
 * This function generates a DM log message when a pd_cmd_err
 * is recvd by PDAPI
 *
 * Parameters: 
 * client_id
 * command_type
 * err_value
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pd_cmd_err(uint32 l_ClientId, 
        pdsm_pd_cmd_e_type e_CmdType, 
        pdsm_pd_cmd_err_e_type e_CmdErr)
{
  LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C_type *p_LogPkt;
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C_type*)log_alloc( LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C,
          sizeof(LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C_type));

  if (p_LogPkt) {
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->u_CmdType = (uint8)e_CmdType;
    p_LogPkt->z_CmdErrType = e_CmdErr;
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->u_Reserved[0] =0;
    p_LogPkt->u_Reserved[1] =0;
    log_commit(p_LogPkt);
  }

}
/*
 ******************************************************************************
 * sm_log_pa_cmd_err
 *
 * Function description:
 *
 * This function generates a DM log message when a pa_cmd_err
 * is recvd by PDAPI
 *
 * Parameters: 
 * l_ClientId
 * command_type
 * err_value
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pa_cmd_err(uint32 l_ClientId, 
                        pdsm_pa_cmd_e_type e_CmdType, 
                        pdsm_pa_cmd_err_e_type e_CmdErr)
{
   LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C_type *p_LogPkt;
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C_type*)log_alloc( LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C,
          sizeof(LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C_type));

  if (p_LogPkt) {
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->u_CmdType = (uint8)e_CmdType;    
    p_LogPkt->z_CmdErrType = e_CmdErr;
    p_LogPkt->u_PacketVersion =0; 
    p_LogPkt->u_Reserved[0] =0; 
    p_LogPkt->u_Reserved[1] =0;
    log_commit(p_LogPkt);
  }
}


/*
 ******************************************************************************
 * sm_log_diag_position
 *
 * Function description:
 *
 * This function generates a DM log message when diagnostic position report is recvd by the
 * PDAPI
 *
 * Parameters: 
 * l_ClientId - l_ClientId 
 * p_PdInfoType - pointer to position
 * pos_from_oem - 1 if the position is received from the OEM, 0 if the position is from position engine.
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_diag_position(uint32 l_ClientId,
                           const pdsm_pd_ext_pos_s_type *p_PdExtPosType, 
                           uint8 u_PosFromOem, 
                           boolean sft, boolean kf)
{
  LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type *p_LogPkt;
  byte i=0;
  pdsm_pd_ext_pos_data_s_type           p_ExtPosData; /* extended position report */    
  
  if (p_PdExtPosType == NULL) 
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type*)log_alloc( LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C,
    sizeof(LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type) );

  if (p_LogPkt) {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_7;
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->l_Longitude = p_PdExtPosType->pd_data.lon;
    p_LogPkt->l_Latitude = p_PdExtPosType->pd_data.lat;
    p_LogPkt->x_Altitude = p_PdExtPosType->pd_data.altitude;
    p_LogPkt->q_TimeStamp = p_PdExtPosType->pd_data.time_stamp;
    p_LogPkt->u_LocUncAng = p_PdExtPosType->pd_data.loc_uncertainty_ang;
    p_LogPkt->u_LocUncA  = p_PdExtPosType->pd_data.loc_uncertainty_a;
    p_LogPkt->u_LocUncP = p_PdExtPosType->pd_data.loc_uncertainty_p;
    p_LogPkt->u_OptFieldMask = p_PdExtPosType->pd_data.opt_field_mask;
    p_LogPkt->w_Heading = p_PdExtPosType->pd_data.heading;
    p_LogPkt->w_VelocityHor = p_PdExtPosType->pd_data.velocity_hor;
    p_LogPkt->u_FixType = p_PdExtPosType->pd_data.fix_type;
    p_LogPkt->s_VelocityVer = p_PdExtPosType->pd_data.velocity_ver;
    p_LogPkt->u_LocUncertaintyV = p_PdExtPosType->pd_data.loc_uncertainty_v;
    p_LogPkt->s_utcOffset = p_PdExtPosType->pd_data.gpsUtcOffset;
    p_LogPkt->u_UncEllConfidence = p_PdExtPosType->pd_data.loc_uncertainty_conf;

    switch (p_PdExtPosType->pd_data.position_mode)
    {
        case PDSM_PD_POSITION_MODE_MSBASED:
          p_LogPkt->u_PositionMethod = 0;
          break;

        case PDSM_PD_POSITION_MODE_MSASSISTED:
          p_LogPkt->u_PositionMethod = 1;
          break;

        case PDSM_PD_POSITION_MODE_STANDALONE:
          p_LogPkt->u_PositionMethod = 2;
          break;

        case PDSM_PD_POSITION_MODE_UNKNOWN:
          p_LogPkt->u_PositionMethod = 3;
          break;

        default:
          p_LogPkt->u_PositionMethod = 0;
          break;
    }

    p_LogPkt->u_PositionSource = 0;  /* Initialize */

    switch (p_PdExtPosType->pd_data.position_type)
    {
      case PDSM_PD_POSITION_TYPE_NEW:
        p_LogPkt->u_PositionSource = 1;
        break;

      default:
        break;

    }

    if ( p_PdExtPosType->pd_data.position_type != PDSM_PD_POSITION_TYPE_INVALID)
    {
      switch (p_PdExtPosType->pd_data.positioning_source )
      {
      case PDSM_PD_POSITION_SOURCE_CELLID:
         p_LogPkt->u_PositionSource |= POSITION_SOURCE_CELLID;
         break;

      case PDSM_PD_POSITION_SOURCE_DEFAULT:
         p_LogPkt->u_PositionSource |=  POSITION_SOURCE_DEFAULT;
         break;

      case PDSM_PD_POSITION_SOURCE_UNKNOWN:
         p_LogPkt->u_PositionSource |= POSITION_SOURCE_UNKNOWN;
         break;

      default:
         /* above are mutually exclusive, but if the fix is computed using SVs
         then it may have used both GPS and GLO SVs, so check for both individually */
         if(PDSM_PD_POSITION_SOURCE_GPS & p_PdExtPosType->pd_data.positioning_source)
         {
            p_LogPkt->u_PositionSource |= POSITION_SOURCE_GPS;
            if (sft == TRUE)
            {
               p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
            }
         }

         if(PDSM_PD_POSITION_SOURCE_GLO & p_PdExtPosType->pd_data.positioning_source)
         {
            p_LogPkt->u_PositionSource |= POSITION_SOURCE_GLO;
            if (sft == TRUE)
            {
               p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
            }
         }
         break;
      }

    }
    if (kf == TRUE)
    {
      p_LogPkt->u_PositionSource |= POSITION_SOURCE_KF_WLS;
    }

    p_ExtPosData = p_PdExtPosType->pd_data.ext_pos_data;
    p_LogPkt->q_ExtLoclat = (int32)p_ExtPosData.lat;
    p_LogPkt->q_ExtLoclon = (int32)p_ExtPosData.lon;
    p_LogPkt->w_ExtLocUncA =  (uint16)p_ExtPosData.alongAxisUnc;
    p_LogPkt->w_ExtLocUncP = (uint16)p_ExtPosData.perpAxisUnc;
    p_LogPkt->q_VelHoriz = (int32)p_ExtPosData.velHor;
    p_LogPkt->q_VelVert  = (int32)p_ExtPosData.velVert;
    p_LogPkt->q_TimeStampTowMs = (uint32)p_ExtPosData.timestamp_tow_ms;
    p_LogPkt->w_TimeStampGpsWeek = (uint16)p_ExtPosData.timestamp_gps_week;
    p_LogPkt->u_PosReportedToNetwork = (uint8)p_ExtPosData.pos_reported_to_network;
    p_LogPkt->w_VelUncHoriz = (uint32)p_ExtPosData.velUncHoriz;
    p_LogPkt->w_VelUncVert = (uint32)p_ExtPosData.velUncVert;
    p_LogPkt->q_FixTimeRemainingMSec = p_PdExtPosType->fixTimeRemaining;    /* Time remaining for the Fix to Timeout */

    p_LogPkt->u_PosFromOem = u_PosFromOem;
    p_LogPkt->q_Hdop = (uint32)p_ExtPosData.h_dop;               
    p_LogPkt->q_Pdop = (uint32)p_ExtPosData.p_dop;               
    p_LogPkt->q_Vdop = (uint32)p_ExtPosData.v_dop;               
    p_LogPkt->u_UtcHour = p_ExtPosData.utc_time.hour; 
    p_LogPkt->u_UtcMin = p_ExtPosData.utc_time.minute; 
    p_LogPkt->w_UtcSec = p_ExtPosData.utc_time.sec; 
    p_LogPkt->q_UtcDate = p_ExtPosData.utc_time.date; 
    p_LogPkt->q_LocUncHorizontal = p_ExtPosData.loc_unc_horizontal;           
    p_LogPkt->w_XtraStartGpsMinutes = p_ExtPosData.xtra_start_gps_minutes;
    p_LogPkt->w_XtraStartGpsWeek = p_ExtPosData.xtra_start_gps_week;
    p_LogPkt->w_XtravalidDurationHours = p_ExtPosData.xtra_valid_duration_hours;
    p_LogPkt->q_SvIdGps = p_PdExtPosType->pd_data.z_NavSvInfo.q_GpsSvMaskUsed;
    p_LogPkt->q_SvIdGlo = p_PdExtPosType->pd_data.z_NavSvInfo.q_GloSvMaskUsed;
    p_LogPkt->t_SvIdBds = p_PdExtPosType->pd_data.z_NavSvInfo.t_BdsSvMaskUsed;
    p_LogPkt->t_SvIdGal = p_PdExtPosType->pd_data.z_NavSvInfo.t_GalSvMaskUsed;
    p_LogPkt->u_SvIdQzss = p_PdExtPosType->pd_data.z_NavSvInfo.u_QzssSvMaskUsed;
    
    for (i=0; i< 8; i++)
      p_LogPkt->u_Reserved[i] = 0;

    log_commit(p_LogPkt);
  }
}


/*
 ******************************************************************************
 * sm_log_nmea_position_report
 *
 * Function description:
 *
 * This function generates a DM log message when nmea report is sent to the
 * PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_ext_nmea_position_report(uint32 l_ClientId,
                           const pdsm_pd_ext_nmea_s_type *p_ExtNMEAType)
{
  LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C_type *p_LogPkt;
  uint8 i=0;

  if (p_ExtNMEAType == NULL) 
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C_type*)log_alloc( LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C,
    sizeof(LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C_type) );
  
  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_2; 
    p_LogPkt->q_ClientId = l_ClientId ;
    p_LogPkt->q_NmeaSentenceType = p_ExtNMEAType->nmea_type;
    p_LogPkt->u_NmeaSentenceLength = (uint8)p_ExtNMEAType->nmea_length;
    for (i=0 ; i< p_ExtNMEAType->nmea_length; i++) 
    {
      p_LogPkt->u_NmeaData[i] = p_ExtNMEAType->nmea_data[i];
    }  
    log_commit(p_LogPkt);
  }
  return;
}

/*
 ******************************************************************************
 * sm_log_ext_meas_report
 *
 * Function description:
 *
 * This function generates a DM log message when nmea report is sent to the
 * PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_ext_meas_report(uint32 l_ClientId,
                            const pdsm_pd_ext_meas_s_type *p_ExtMeasType)
{
  LOG_GNSS_PDSM_EXT_STATUS_MEAS_REPORT_C_type *p_LogPkt;
  uint8 i=0;

  if (p_ExtMeasType == NULL) 
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_GNSS_PDSM_EXT_STATUS_MEAS_REPORT_C_type*)log_alloc( LOG_GNSS_PDSM_EXT_STATUS_MEAS_REPORT_C,
    sizeof(LOG_GNSS_PDSM_EXT_STATUS_MEAS_REPORT_C_type) );
  
  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1; /* new callisto logs start at version 0 */
    p_LogPkt->q_ClientId = l_ClientId ;
    p_LogPkt->q_EphSvmask = p_ExtMeasType->eph_svmask;      /* sv mask for the ephemeris */
    p_LogPkt->q_AlmSvmask = p_ExtMeasType->alm_svmask;      /* sv mask for the ephemeris */
    p_LogPkt->q_Ionovalid = p_ExtMeasType->iono_valid; /* IONO is Valid */
    p_LogPkt->q_HealthSvmask = p_ExtMeasType->health_svmask; /* SV mask for the health. if the bit is set, health for that SV is good */
    p_LogPkt->q_VisibleSvmask = p_ExtMeasType->visible_svmask;
    p_LogPkt->q_Timeuncms = p_ExtMeasType->time_unc_ms; /* Time uncertainty in milli seconds, floor at 20 ms */

    p_LogPkt->u_NumSvs = MIN(p_ExtMeasType->num_svs, GNSS_ARRAY_SIZEOF(p_LogPkt->z_MeasData));   
    /* i should be less than num of elements in array p_LogPkt->z_MeasData */
    for (i =0 ; i < p_LogPkt->u_NumSvs ; i++) 
    {
      p_LogPkt->z_MeasData[i].w_SvId = p_ExtMeasType->ext_meas_report_type[i].sv_id; 
      p_LogPkt->z_MeasData[i].w_CNo = p_ExtMeasType->ext_meas_report_type[i].c_no;
      p_LogPkt->z_MeasData[i].q_Latency = p_ExtMeasType->ext_meas_report_type[i].latency_ms;
      p_LogPkt->z_MeasData[i].q_Azimuth = (uint32)p_ExtMeasType->ext_meas_report_type[i].azimuth;
      p_LogPkt->z_MeasData[i].q_Elevation = (uint32)p_ExtMeasType->ext_meas_report_type[i].elevation;
      p_LogPkt->z_MeasData[i].u_Status = (uint8)p_ExtMeasType->ext_meas_report_type[i].sv_state;
    }
    for (i=0 ; i< 4; i++) 
    {
      p_LogPkt->q_Reserved[i] = 0;
    }  
    log_commit(p_LogPkt);
  }
  return;
  
}



/*
 ******************************************************************************
 * sm_log_on_demand_start
 *
 * Function description:
 *
 * This function generates a DM log message when on-demand positioning is started
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_on_demand_start(uint8 u_Mode, float f_Punc, float f_Tunc)
{
  LOG_CGPS_PDSM_ON_DEMAND_SESSION_START_C_type *p_LogPkt;
  uint8 i =0;
  
  p_LogPkt = (LOG_CGPS_PDSM_ON_DEMAND_SESSION_START_C_type*)log_alloc( LOG_CGPS_PDSM_ON_DEMAND_SESSION_START_C,
    sizeof(LOG_CGPS_PDSM_ON_DEMAND_SESSION_START_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->u_OnDemandMode = u_Mode;
    switch(u_Mode)
    {
      case 1:
        p_LogPkt->u_GpsSessionTimeout = 3;
        p_LogPkt->q_Accuracy = 1000;
        break;
      case 2:
        p_LogPkt->u_GpsSessionTimeout = 15;
        p_LogPkt->q_Accuracy = 1000;
        break;
      case 3:
        p_LogPkt->u_GpsSessionTimeout = 30;
        p_LogPkt->q_Accuracy = 1000;
        break;
      case 4:
        p_LogPkt->u_GpsSessionTimeout = 75;
        p_LogPkt->q_Accuracy = 1000;
        break;
      case 5:
        p_LogPkt->u_GpsSessionTimeout = 1;
        p_LogPkt->q_Accuracy = 1000;
        break;
      case 6:
        p_LogPkt->u_GpsSessionTimeout = 3;
        p_LogPkt->q_Accuracy = 1000;
        break;
      case 7:
        p_LogPkt->u_GpsSessionTimeout = 3;
        p_LogPkt->q_Accuracy = 1000;
        break;
      default:
        break;

    }
    p_LogPkt->f_Punc = *((uint32 *)((void *)&f_Punc));
    p_LogPkt->f_Tunc = *((uint32 *)((void *)&f_Tunc));
    for (i=0; i<4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
  return;
}


/*
 ******************************************************************************
 * sm_log_on_demand_stop
 *
 * Function description:
 *
 * This function generates a DM log message when on-demand positioning is stopped
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_on_demand_stop(
          uint8  u_OnDemandMode,
          uint8  u_Reason,
          uint32 q_NumFailedFixes )
{
  LOG_CGPS_PDSM_ON_DEMAND_SESSION_STOP_C_type *p_LogPkt;
  uint8 i =0;
  
  p_LogPkt = (LOG_CGPS_PDSM_ON_DEMAND_SESSION_STOP_C_type*)log_alloc( LOG_CGPS_PDSM_ON_DEMAND_SESSION_STOP_C,
    sizeof(LOG_CGPS_PDSM_ON_DEMAND_SESSION_STOP_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->u_OnDemandMode = u_OnDemandMode;
    p_LogPkt->u_Reason = u_Reason;
    p_LogPkt->q_NumFailedFixes = q_NumFailedFixes;
    
    for (i=0; i<4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
  return;
}


/*
 ******************************************************************************
 * sm_log_on_demand_not_started
 *
 * Function description:
 *
 * This function generates a DM log message when on-demand positioning is scheduled
 * to be started but not started
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_on_demand_not_started(
          uint8  u_OnDemandMode,
          uint8  u_Reason,
          uint32 q_NumNoFixes,
          float f_Punc, 
          float f_Tunc
)
{
  LOG_CGPS_PDSM_ON_DEMAND_SESSION_NOT_STARTED_C_type *p_LogPkt;
  uint8 i =0;
  
  p_LogPkt = (LOG_CGPS_PDSM_ON_DEMAND_SESSION_NOT_STARTED_C_type*)log_alloc( LOG_CGPS_PDSM_ON_DEMAND_SESSION_NOT_STARTED_C,
    sizeof(LOG_CGPS_PDSM_ON_DEMAND_SESSION_NOT_STARTED_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->u_OnDemandMode = u_OnDemandMode;
    p_LogPkt->q_NumFailedFixes = q_NumNoFixes;
    p_LogPkt->f_Punc = *((uint32 *)((void *)&f_Punc));
    p_LogPkt->f_Tunc = *((uint32 *)((void *)&f_Tunc));
    for (i=0; i<4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
  return;
}

/*
 ******************************************************************************
 * sm_log_extern_coarse_pos_inj_start
 *
 * Function description:
 *
 * This function generates a DM log message when external coarse position 
 * injection is started
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_extern_coarse_pos_inj_start( pdsm_cmd_s_type*  p_cmd_type )
{

  LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type *p_LogPkt;
  uint8 i =0;
  pdsm_pd_external_position_info_type *ext_pos_ptr;
  
  ext_pos_ptr = &p_cmd_type->cmd.pd.info.ext_pos_info;

  p_LogPkt = (LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type*)log_alloc( LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C,
    sizeof(LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_2;
    p_LogPkt->q_ClientId = p_cmd_type->cmd.pd.client_id;
    p_LogPkt->q_opt_field_mask = ext_pos_ptr->opt_field_mask;
    p_LogPkt->q_time_info_type = ext_pos_ptr->time_info_type;
    p_LogPkt->q_timestamp_sec =  ext_pos_ptr->timestamp_sec;

    p_LogPkt->q_alt_def = ext_pos_ptr->alt_def;
    p_LogPkt->q_source = ext_pos_ptr->source;
    p_LogPkt->d_latitude =  ext_pos_ptr->latitude;
    p_LogPkt->d_longitude = ext_pos_ptr->longitude;
    p_LogPkt->f_loc_unc_hor = ext_pos_ptr->loc_unc_hor;
    p_LogPkt->f_altitude =  ext_pos_ptr->altitude;
    p_LogPkt->f_loc_unc_vert = ext_pos_ptr->loc_unc_vert;
    p_LogPkt->u_confidence_hor =  ext_pos_ptr->confidence_hor;
    p_LogPkt->u_confidence_vert =  ext_pos_ptr->confidence_vert;

    for (i=0; i<4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);

  }
}

/*
 ******************************************************************************
 * sm_log_get_gps_state_info
 *
 * Function description:
 *
 * This function generates a DM log message when the receiver state info is
 * reported to PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_get_gps_state_info( uint32 q_ClientId )
{

  LOG_CGPS_PDSM_GPS_STATE_INFO_C_type *p_LogPkt;


  p_LogPkt = (LOG_CGPS_PDSM_GPS_STATE_INFO_C_type*)log_alloc( LOG_CGPS_PDSM_GPS_STATE_INFO_C,
     sizeof(LOG_CGPS_PDSM_GPS_STATE_INFO_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->q_ClientId = q_ClientId;
    log_commit(p_LogPkt);

  }
}

/*
 ******************************************************************************
 * sm_log_get_best_avail_pos_info
 *
 * Function description:
 *
 * This function generates a DM log message when the best available position info
 * is reported to PDAPI from the measurement engine.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_get_best_avail_pos_info( uint32 q_ClientId)
{

  LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type *p_LogPkt;
  uint32 i = 0;
  p_LogPkt = (LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type*)log_alloc( LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C,
     sizeof(LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->q_ClientId = q_ClientId;
    for (i=0; i<4;i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
}


/*
 ******************************************************************************
 * sm_log_report_ext_status_best_avail_pos_info
 *
 * Function description:
 *
 * This function generates a DM log message when the best available position is
 * reported to PDAPI from measurement engine.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_report_ext_status_best_avail_pos_info(uint32 q_ClientId, 
                           const pdsm_pd_ext_pos_rpt_s_type *p_ExtBestAvailPosType)
{
  LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type *p_LogPkt;
  uint32 i = 0;

  p_LogPkt = (LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type*)log_alloc( LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C,
     sizeof(LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type) );

  if (p_LogPkt)
  {
    MSG_HIGH("log_alloc success ", 0,0,0);
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->q_ClientId = q_ClientId;
    /* Copy the position report information from the extended status position report to the DM log packet position report */
    p_LogPkt->sessionStatus = p_ExtBestAvailPosType->sessionStatus;
    p_LogPkt->sessionId = p_ExtBestAvailPosType->sessionId;
    /*****  The position report information  which is reported to the QMI client ****/
    /*  Latitude */
    p_LogPkt->latitude_valid = p_ExtBestAvailPosType->latitude_valid;
    p_LogPkt->latitude = p_ExtBestAvailPosType->latitude; 
    /*   Longitude */
    p_LogPkt->longitude_valid = p_ExtBestAvailPosType->longitude_valid;
    p_LogPkt->longitude = p_ExtBestAvailPosType->longitude;
  
    /*   Circular Horizontal Position Uncertainty */
    p_LogPkt->horUncCircular_valid = p_ExtBestAvailPosType->horUncCircular_valid;
    p_LogPkt->horUncCircular = p_ExtBestAvailPosType->horUncCircular;
    
    /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
    p_LogPkt->horUncEllipseSemiMinor_valid = p_ExtBestAvailPosType->horUncEllipseSemiMinor_valid;
    p_LogPkt->horUncEllipseSemiMinor = p_ExtBestAvailPosType->horUncEllipseSemiMinor;

    /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
    p_LogPkt->horUncEllipseSemiMajor_valid = p_ExtBestAvailPosType->horUncEllipseSemiMajor_valid;
    p_LogPkt->horUncEllipseSemiMajor = p_ExtBestAvailPosType->horUncEllipseSemiMajor;

    /*  Elliptical Horizontal Uncertainty Azimuth */
    p_LogPkt->horUncEllipseOrientAzimuth_valid = p_ExtBestAvailPosType->horUncEllipseOrientAzimuth_valid;
    p_LogPkt->horUncEllipseOrientAzimuth = p_ExtBestAvailPosType->horUncEllipseOrientAzimuth;
  
    /*  Horizontal Circular Confidence */
    p_LogPkt->horCircularConfidence_valid = p_ExtBestAvailPosType->horCircularConfidence_valid;
    p_LogPkt->horCircularConfidence = p_ExtBestAvailPosType->horCircularConfidence;
  
    /*  Horizontal Reliability */
    p_LogPkt->horReliability_valid = p_ExtBestAvailPosType->horReliability_valid;
    p_LogPkt->horReliability = p_ExtBestAvailPosType->horReliability;
  
    /*  Horizontal Speed */
    p_LogPkt->horSpeed_valid = p_ExtBestAvailPosType->horSpeed_valid;
    p_LogPkt->horSpeed = p_ExtBestAvailPosType->horSpeed;
  
    /*  Horizontal Speed Uncertainty */
    p_LogPkt->horSpeedUnc_valid = p_ExtBestAvailPosType->horSpeedUnc_valid; 
    p_LogPkt->horSpeedUnc = p_ExtBestAvailPosType->horSpeedUnc;
  
    /*  Altitude With Respect to Ellipsoid */
    p_LogPkt->altitudeWrtEllipsoid_valid = p_ExtBestAvailPosType->altitudeWrtEllipsoid_valid;
    p_LogPkt->altitudeWrtEllipsoid = p_ExtBestAvailPosType->altitudeWrtEllipsoid;
  
    /*  Altitude With Respect to Sea Level */
    p_LogPkt->altitudeWrtMeanSeaLevel_valid = p_ExtBestAvailPosType->altitudeWrtMeanSeaLevel_valid;
    p_LogPkt->altitudeWrtMeanSeaLevel = p_ExtBestAvailPosType->altitudeWrtMeanSeaLevel;
  
    /*  Vertical Uncertainty */
    p_LogPkt->vertUnc_valid = p_ExtBestAvailPosType->vertSpeedUnc_valid;
    p_LogPkt->vertUnc = p_ExtBestAvailPosType->vertUnc;

    /*  Vertical Confidence */
    p_LogPkt->vertConfidence_valid = p_ExtBestAvailPosType->vertConfidence_valid;
    p_LogPkt->vertConfidence = p_ExtBestAvailPosType->vertConfidence;

    /*  Vertical Reliability */
    p_LogPkt->vertReliability_valid = p_ExtBestAvailPosType->vertReliability_valid;
    p_LogPkt->vertReliability = p_ExtBestAvailPosType->vertReliability;
  
    /*  Vertical Speed */
    p_LogPkt->vertSpeed_valid = p_ExtBestAvailPosType->vertSpeed_valid;
    p_LogPkt->vertSpeed = p_ExtBestAvailPosType->vertSpeed;
  
    /*  Vertical Speed Uncertainty */
    p_LogPkt->vertSpeedUnc_valid = p_ExtBestAvailPosType->vertSpeedUnc_valid;
    p_LogPkt->vertSpeedUnc = p_ExtBestAvailPosType->vertSpeedUnc;
  
    /*  Heading */
    p_LogPkt->heading_valid = p_ExtBestAvailPosType->heading_valid;
    p_LogPkt->heading = p_ExtBestAvailPosType->heading;
  
    /*  Heading Uncertainty */
    p_LogPkt->headingUnc_valid = p_ExtBestAvailPosType->headingUnc_valid;
    p_LogPkt->headingUnc = p_ExtBestAvailPosType->headingUnc;
  
    /*  Magnetic Deviation */
    p_LogPkt->magneticDeviation_valid = p_ExtBestAvailPosType->magneticDeviation_valid;
    p_LogPkt->magneticDeviation = p_ExtBestAvailPosType->magneticDeviation;
  
    /*  Technology Used */
    p_LogPkt->technologyMask_valid = p_ExtBestAvailPosType->technologyMask_valid;
    p_LogPkt->technologyMask = p_ExtBestAvailPosType->technologyMask;

    /*  Dilution of Precision */
    p_LogPkt->PDOP_valid = p_ExtBestAvailPosType->DOP_valid;
    p_LogPkt->PDOP = p_ExtBestAvailPosType->DOP.PDOP;
  
    p_LogPkt->HDOP_valid = p_ExtBestAvailPosType->DOP_valid;
    p_LogPkt->HDOP = p_ExtBestAvailPosType->DOP.HDOP;
    
    p_LogPkt->VDOP_valid = p_ExtBestAvailPosType->DOP_valid;
    p_LogPkt->VDOP = p_ExtBestAvailPosType->DOP.VDOP;
  
    /*  UTC Timestamp */
    p_LogPkt->timestampUtc_valid = p_ExtBestAvailPosType->timestampUtc_valid;
    p_LogPkt->timestampUtc = p_ExtBestAvailPosType->timestampUtc;

    if( (p_ExtBestAvailPosType->timestampUtc_valid == TRUE) &&
        (p_ExtBestAvailPosType->gpsTime_valid == TRUE)
      )
    {
      p_LogPkt->leapSeconds = (uint8)((uint32)((p_ExtBestAvailPosType->gpsTime.gpsTimeOfWeekMs - 
                                (uint32)p_ExtBestAvailPosType->timestampUtc))/1000);
      p_LogPkt->leapSeconds_valid = TRUE;
        
    }
    else
    {
      p_LogPkt->leapSeconds_valid = FALSE;
    }

    /*  Time Uncertainty */
    p_LogPkt->timeUnc_valid = p_ExtBestAvailPosType->timeUnc_valid;
    p_LogPkt->timeUnc = p_ExtBestAvailPosType->timeUnc;

    /*  Time Source */
    p_LogPkt->timeSrc_valid = p_ExtBestAvailPosType->timeSrc_valid;
    p_LogPkt->timeSrc = p_ExtBestAvailPosType->timeSrc;

    /*  Sensor Data Usage */
    p_LogPkt->sensorDataUsage_valid = p_ExtBestAvailPosType->sensorDataUsage_valid;
    p_LogPkt->sensorUsageMask = p_ExtBestAvailPosType->sensorDataUsage.usageMask;
    p_LogPkt->sesorAidingIndicatorMask = p_ExtBestAvailPosType->sensorDataUsage.aidingIndicatorMask;
  
    /*  SV's Used to Calculate the Fix */
    p_LogPkt->gnssSvUsedList_valid = p_ExtBestAvailPosType->gnssSvUsedList_valid;  
    p_LogPkt->gnssSvUsedList_len = p_ExtBestAvailPosType->gnssSvUsedList_len; 
    memscpy((void*)p_LogPkt->gnssSvUsedList, sizeof(p_LogPkt->gnssSvUsedList),
		    p_ExtBestAvailPosType->gnssSvUsedList, sizeof(p_ExtBestAvailPosType->gnssSvUsedList));

    /*  Horizontal Elliptical Confidence */
    p_LogPkt->horEllipticalConfidence_valid = p_ExtBestAvailPosType->horEllipticalConfidence_valid;
    p_LogPkt->horEllipticalConfidence = p_ExtBestAvailPosType->horEllipticalConfidence;
  
    /*  GPS Time */
    p_LogPkt->gpsTime_valid = p_ExtBestAvailPosType->gpsTime_valid;
    p_LogPkt->gpsWeek = p_ExtBestAvailPosType->gpsTime.gpsWeek;
    p_LogPkt->gpsTimeOfWeekMs = p_ExtBestAvailPosType->gpsTime.gpsTimeOfWeekMs;
  
    /* End of position report information */
    for (i=0; i<4;i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt); 
  }
  else
  {
    MSG_HIGH("log_alloc failure", 0,0,0);
  }
}

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif
/*
 ******************************************************************************
 * sm_log_report_gps_state_info
 *
 * Function description:
 *
 * This function generates a DM log message when the receiver state info is
 * reported to PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_report_ext_status_gps_state_info(uint32 q_ClientId, 
                           const pdsm_pd_ext_gps_state_s_type *p_ExtGpsStateType)
{

  LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type *p_LogPkt;
  uint8 i = 0;

  p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type*)log_alloc( LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C,
     sizeof(LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type) );

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_2;
    p_LogPkt->q_ClientId = q_ClientId;
    p_LogPkt->q_Lat = p_ExtGpsStateType->lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
    p_LogPkt->q_Lon = p_ExtGpsStateType->lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
    p_LogPkt->q_Altitude = p_ExtGpsStateType->altitude; /* Altitude in meters */
    p_LogPkt->q_Punc = p_ExtGpsStateType->loc_unc_horizontal; /* Position Unc in meters */
    p_LogPkt->w_LocUncVertical = p_ExtGpsStateType->loc_unc_vertical; /* vertical unc in meters  */
    p_LogPkt->q_TimestampTowMs = p_ExtGpsStateType->timestamp_tow_ms; /* Timestamp in GPS TOW milliseconds */
    p_LogPkt->w_TimestampGpsWeek = p_ExtGpsStateType->timestamp_gps_week;  /*GPS week number */
    p_LogPkt->q_TuncMs = p_ExtGpsStateType->time_unc_ms; /* Time uncertainty in micro seconds */
    p_LogPkt->b_EngineState = p_ExtGpsStateType->engine_state ; /*  GPS engine state on/off */
    p_LogPkt->q_EphSvMask = p_ExtGpsStateType->eph_svmask; /* SV Mask for the ephemeris */
    p_LogPkt->q_AlmSvMask = p_ExtGpsStateType->alm_svmask; /* SV Mask for the Almanac */
    p_LogPkt->q_IonoValid = p_ExtGpsStateType->iono_valid;  /*  IONO validity */
    p_LogPkt->q_VisibleSvMask = p_ExtGpsStateType->visible_svmask;  /*  IONO validity */
    p_LogPkt->q_HealthMask = p_ExtGpsStateType->health_svmask; /* SV Mask for the Health */
    p_LogPkt->w_XtraStartGpsWeek = p_ExtGpsStateType->xtra_start_gps_week; /*  Current XTRA info is good starting this GPS week */
    p_LogPkt->w_XtraStartGpsMinutes = p_ExtGpsStateType->xtra_start_gps_minutes; /*  Current XTRA information is good starting this many minutes into the week */
    for (i=0; i<4;i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);

  }
}


/*===========================================================================
FUNCTION  sm_log_eph_download
DESCRIPTION
This function generates a DM log message for eph download events.

Parameters:
const sm_log_eph_download_data* p_EphDownloadData  
===========================================================================*/
void sm_log_eph_download(const sm_log_eph_download_data* p_EphDownloadData)
{
   LOG_CGPS_PDSM_EPH_DOWNLOAD_C_type*  p_LogPkt;
   uint8 i = 0;

   if (p_EphDownloadData == NULL) 
   {
      return;
   }

   /* Try to allocate memory for the log packet */
   p_LogPkt = (LOG_CGPS_PDSM_EPH_DOWNLOAD_C_type*)log_alloc(
      LOG_CGPS_SM_EPH_RANDOMIZATION_INFO_C,
      sizeof( LOG_CGPS_PDSM_EPH_DOWNLOAD_C_type ) );

   if ( p_LogPkt != 0 ) /* sufficient space and log is enabled */
   {
      p_LogPkt->u_PacketVersion                                = CGPS_SM_LOG_PKT_VERSION_2;
	  p_LogPkt->z_EphDownloadData = *p_EphDownloadData;
      log_commit( p_LogPkt );
   }
}  /* sm_log_eph_download */


/*
 ******************************************************************************
 * sm_log_pos_inj_request
 *
 * Function description:
 *
 * This function generates a DM log message when position inject request info is
 * reported to PDAPI
 *
 * Parameters: 
 *    uint32- clientId of the client requesting position
 *    pdsm_pd_ext_pos_inj_req_s_type - pointer to the data to be sent in the log
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_pos_inj_request(uint32 q_ClientId, 
                           const pdsm_pd_ext_pos_inj_req_s_type *p_ext_pos_inj_req_type)
{
  
   LOG_CGPS_PDSM_EXT_STATUS_POS_INJ_REQ_INFO_C_type*  p_LogPkt;

   if (p_ext_pos_inj_req_type == NULL) 
   {
      return;
   }

   /* Try to allocate memory for the log packet */
   p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_POS_INJ_REQ_INFO_C_type*)log_alloc(
      LOG_CGPS_PDSM_EXT_STATUS_POS_INJ_REQ_INFO_C,
      sizeof( LOG_CGPS_PDSM_EXT_STATUS_POS_INJ_REQ_INFO_C_type) );

   if ( p_LogPkt != 0 ) /* sufficient space and log is enabled */
   {
     p_LogPkt->u_PacketVersion  = CGPS_SM_LOG_PKT_VERSION_1;
     p_LogPkt->q_ClientId = q_ClientId;
     p_LogPkt->q_flags = p_ext_pos_inj_req_type->flags;
     p_LogPkt->q_Lat = p_ext_pos_inj_req_type->lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
     p_LogPkt->q_Lon = p_ext_pos_inj_req_type->lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */   
     p_LogPkt->q_Punc = p_ext_pos_inj_req_type->pos_unc; /* Position Unc in meters */
     p_LogPkt->q_TimestampTowMs = p_ext_pos_inj_req_type->timestamp_tow_ms; /* Position Unc in meters */
     p_LogPkt->w_TimestampGpsWeek = p_ext_pos_inj_req_type->timestamp_gps_week; /* Position Unc in meters */
     log_commit( p_LogPkt );
   }
}
   

/**
* writes out a given uint32 as little endian to given byte array
* @param pOutputByteArray
   array where data will be written out to
* @param data 
   uint32 data to be written out
* @returns boolean
   false if pOutputByteArray == NULL. True otherwise.
  @notes Caller must ensure that pOutputByteArray has enough space
*/
boolean sm_log_write_uint32_little_endian(uint8 *pOutputByteArray, uint32 data)
{
   boolean b_retval = (NULL != pOutputByteArray);
   uint8 u_byte_iter = 0;
   if(b_retval)
   {
      for(u_byte_iter = 0; u_byte_iter < sizeof(data); u_byte_iter++)
      {
         pOutputByteArray[u_byte_iter]  = SM_LOG_EXTRACT_BYTE(u_byte_iter, data) ;
      }
   }
   return b_retval;
}


/** logs an error message generated within SM.
 * 
 * @param p_error_msg
 * pointer to error message string to be outputted.
 * 
 * @param u_severity
 *  user defined numeric value denoting severity of error (maybe
 *  meaningful only to SM)
 * 
 * @param p_filename
 * pointer to filename where error message occured
 * 
 * @param q_linenum
 * line number of error
 */
void sm_log_error_message(const char* p_error_msg, 
                          uint8 u_severity, 
                          const char* p_filename, 
                          uint32 q_linenum)
{
   if(!p_filename)
   {
      p_filename = "null";
   }

   if(p_error_msg)
   {
      LOG_GNSS_SM_ERROR_C_type *p_LogPkt = NULL;
      uint32 q_filename_length = 0;
      char c_temp = 0;
      uint32 q_message_length = 0;
      int32 n_retval = 0;

      /* estimate size of "filename:line " text */
      n_retval = (int32)(strlen(p_filename) + snprintf(&c_temp, 0, ":%lu ", q_linenum));
      q_filename_length = n_retval >= 0 ? n_retval : 0;
      q_message_length = (uint32) (strlen(p_error_msg)+ q_filename_length);
      q_message_length = MIN(q_message_length, 
                              SM_LOG_LENGTH_MAX_SM_ERROR_MESSAGE-1);
                              /* -1 to keep u_length below from overflowing */

      p_LogPkt = (LOG_GNSS_SM_ERROR_C_type*)
                     log_alloc(LOG_GNSS_SM_ERROR_C,
                               sizeof(LOG_GNSS_SM_ERROR_C_type) + q_message_length);
      if(p_LogPkt)
      {
         p_LogPkt->u_PacketVersion = GNSS_GEN8_LOG_VERSION;
         p_LogPkt->q_Reserved[0] = 0;
         p_LogPkt->q_Reserved[1] = 0;
         p_LogPkt->u_Length = (uint8) (q_message_length + 1); 
         /* +1 because of LOG_GNSS_SM_ERROR_C_type::u_MessageData[1] */

         p_LogPkt->u_Severity = u_severity;

         if(n_retval >= 0)
         {
            /* if message + filename / line number is larger, hopefully, filename + line number will
            give us enough clues about the error */
            (void) snprintf((char*)p_LogPkt->u_MessageData, p_LogPkt->u_Length, "%s:%lu %s", 
                p_filename , 
                q_linenum, 
                p_error_msg);
         }
         else
         {
            memscpy(p_LogPkt->u_MessageData, p_LogPkt->u_Length-1, p_error_msg, p_LogPkt->u_Length-1);
         }
         p_LogPkt->u_MessageData[p_LogPkt->u_Length - 1] = '\0'; /* nul terminate string */
         
         log_commit(p_LogPkt);
      } /* end if(p_LogPkt) */
   } /* end if(p_error_msg) */
   return;
}

/*
 ******************************************************************************
 * sm_log_oemdre_measurement_report
 *
 * Function description:
 *
 * This function generates a DM log message when measurement report is sent to the
 * OEM
 *
 * Parameters:
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_oemdre_measurement_report(const sm_GpsMeasRptStructType* p_measReport)
{
  LOG_GNSS_OEMDRE_MEASUREMENT_REPORT_C_type* p_LogPkt;
  gnss_TimeStructType_DmLog*                 pz_DstTime;
  const gnss_TimeStructType*                 pz_SrcTime;
  gnss_ClockStructType_DmLog*                pz_DstClk;
  const gnss_ClockStructType*                pz_SrcClk;
  gps_CDMAClockInfoStructType_ForDmLog*      pz_DstCDMAClk;
  const gps_CDMAClockInfoStructType*         pz_SrcCDMAClk;
  uint8 i=0;
  volatile FLT f_data;

  if (p_measReport == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet LOG_GNSS_OEMDRE_MEASUREMENT_REPORT_C*/
  p_LogPkt = (LOG_GNSS_OEMDRE_MEASUREMENT_REPORT_C_type*)log_alloc( LOG_GNSS_OEMDRE_MEASUREMENT_REPORT_C,
    sizeof(LOG_GNSS_OEMDRE_MEASUREMENT_REPORT_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion                  = GNSS_GEN8_LOG_VERSION_OEMDRE_V2;

    p_LogPkt->e_MeasRptReason                  = p_measReport->e_MeasRptReason;
    p_LogPkt->z_MeasBlk.u_NumSvs               = p_measReport->z_MeasBlk.u_NumSvs;
    p_LogPkt->z_MeasBlk.u_SeqNum               = p_measReport->z_MeasBlk.u_SeqNum;
    p_LogPkt->z_MeasBlk.u_MaxMessageNum        = p_measReport->z_MeasBlk.u_MaxMessageNum;
    p_LogPkt->z_MeasBlk.w_RFLoss               = p_measReport->z_MeasBlk.z_Gnss[0].w_RfLoss;

    //Fields for gnss_ClockStructType
    pz_DstClk = &p_LogPkt->z_MeasBlk.z_MeasClock;
    pz_SrcClk = &p_measReport->z_MeasBlk.z_MeasClock;
    pz_DstClk->u_SystemRtcValid                = pz_SrcClk->u_SystemRtcValid;
    pz_DstClk->q_RefFCount                     = pz_SrcClk->q_RefFCount;
    pz_DstClk->q_NumClockResets                = pz_SrcClk->q_NumClockResets;
    pz_DstClk->t_SystemRtcMs                   = pz_SrcClk->t_SystemRtcMs;

    //Fields for gnss_TimeStructType
    pz_DstTime = &pz_DstClk->z_Time;
    pz_SrcTime = &pz_SrcClk->z_Time;
    pz_DstTime->u_DeltaLeapSec                 = pz_SrcTime->z_GnssTimeBias.u_DeltaLeapSec;
    pz_DstTime->u_DeltaLeapSecUnc              = pz_SrcTime->z_GnssTimeBias.u_DeltaLeapSecUnc;

    SM_MEMCPY(pz_DstTime->f_GGTBMs,            pz_SrcTime->z_GnssTimeBias.z_GpsGlo.f_TBMs);
    SM_MEMCPY(pz_DstTime->f_GGTBUncMs,         pz_SrcTime->z_GnssTimeBias.z_GpsGlo.f_TBUncMs);

    SM_MEMCLR(pz_DstTime->z_GnssTime);
    SM_MEMCLR(pz_DstTime->z_GloTime);

    //Fields for gnss_FreqStructType
    SM_MEMCPY(pz_DstClk->z_Freq.f_ClkFreqBias, pz_SrcClk->z_Freq.f_ClkFreqBias);
    SM_MEMCPY(pz_DstClk->z_Freq.f_ClkFreqUnc,  pz_SrcClk->z_Freq.f_ClkFreqUnc);
    pz_DstClk->z_Freq.e_SourceOfFreq           = pz_SrcClk->z_Freq.e_SourceOfFreq;

    //Fields for gps_CDMAClockInfoStructType
    pz_DstCDMAClk = &p_LogPkt->z_MeasBlk.z_CDMAClockInfo;
    pz_SrcCDMAClk = &p_measReport->z_MeasBlk.z_CDMAClockInfo;
    pz_DstCDMAClk->q_CdmaRtcSync80CCx8         = pz_SrcCDMAClk->q_CdmaRtcSync80CCx8;
    pz_DstCDMAClk->q_GpsRtcSubMsSync80GCx32    = pz_SrcCDMAClk->q_GpsRtcSubMsSync80GCx32;
    pz_DstCDMAClk->q_CdmaRtcLatchedCCx32       = pz_SrcCDMAClk->q_CdmaRtcLatchedCCx32;
    pz_DstCDMAClk->q_GpsRtcLatchedGCx32        = pz_SrcCDMAClk->q_GpsRtcLatchedGCx32;
    pz_DstCDMAClk->q_FCount                    = pz_SrcCDMAClk->q_FCount;

    p_LogPkt->z_MeasBlk.e_MeasBlkSrc           = p_measReport->z_MeasBlk.e_MeasBlkSrc;
    SM_MEMCLR(p_LogPkt->z_MeasBlk.n_Meas);

    if (GNSS_MEAS_BLK_SRC_GPS == p_LogPkt->z_MeasBlk.e_MeasBlkSrc)
    {
      gnss_MeasStructType_ForDmLog* pz_Dst;
      const gnss_MeasStructType*    pz_Src;

      //Fields for gps_TimeStructType
      pz_DstTime->z_GnssTime.w_GpsWeek        = pz_SrcTime->z_GpsTime.w_GpsWeek;
      pz_DstTime->z_GnssTime.q_GpsMsec        = pz_SrcTime->z_GpsTime.q_GpsMsec;

      SM_MEMCPY(pz_DstTime->z_GnssTime.f_ClkTimeBias,
                pz_SrcTime->z_GpsTime.f_ClkTimeBias);

      f_data = pz_SrcTime->z_GpsTime.f_ClkTimeUncMs * 1.0e6f;
      SM_MEMCPY(pz_DstTime->z_GnssTime.f_ClkTimeUncMs, f_data);

      pz_DstTime->z_GnssTime.e_SourceOfTime   = pz_SrcTime->z_GpsTime.e_SourceOfTime;

      //Fields for gnss_GpsMeasStructType
      for(i=0; i < MIN(ARR_SIZE(p_LogPkt->z_MeasBlk.n_Meas.z_Gps),
                       p_measReport->z_MeasBlk.u_NumSvs); i++)
      {
        pz_Dst = &p_LogPkt->z_MeasBlk.n_Meas.z_Gps[i];
        pz_Src = &p_measReport->z_MeasBlk.z_Gnss[i];

        pz_Dst->w_Sv                      = pz_Src->w_Sv;
        pz_Dst->e_SvState                 = pz_Src->q_SvState;
        pz_Dst->u_Observe                 = pz_Src->u_Observe;
        pz_Dst->u_GoodObs                 = pz_Src->u_GoodObs;
        pz_Dst->u_FilterN                 = pz_Src->u_FilterN;
        pz_Dst->u_PreInt                  = pz_Src->u_PreInt;
        pz_Dst->u_CSlipCount              = pz_Src->u_CSlipCount;
        pz_Dst->w_PostInt                 = pz_Src->w_PostInt;
        pz_Dst->q_MeasStatus              = pz_Src->q_MeasStatus;
        pz_Dst->q_MeasStatus2             = pz_Src->q_MeasStatus2;
        pz_Dst->w_Cno                     = pz_Src->w_Cno * 10;
        pz_Dst->x_LatencyMs               = (int16)pz_Src->l_LatencyMs;

        /* Filtered measurement */
        SM_MEMCPY(pz_Dst->z_FiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_FiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_FiltMeas.z_SvTime.q_Ms  = pz_Src->z_FiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvTimeUncMs,
                  pz_Src->z_FiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeed,
                  pz_Src->z_FiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeedUnc,
                  pz_Src->z_FiltMeas.f_SvSpeedUnc);

        /* Un-filtered measurement */
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_UnfiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_UnfiltMeas.z_SvTime.q_Ms    = pz_Src->z_UnfiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvTimeUncMs,
                  pz_Src->z_UnfiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeed,
                  pz_Src->z_UnfiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeedUnc,
                  pz_Src->z_UnfiltMeas.f_SvSpeedUnc);

        pz_Dst->u_MultipathEstValid       = pz_Src->u_MultipathEstValid;
        if (FALSE != pz_Src->u_MultipathEstValid)
        {
          SM_MEMCPY(pz_Dst->f_MultipathEst, pz_Src->f_MultipathEst);
        }

        pz_Dst->u_SvDirValid              = pz_Src->u_SvDirValid;
        if (FALSE != pz_Src->u_SvDirValid)
        {
          SM_MEMCPY(pz_Dst->f_Azimuth,      pz_Src->f_Azimuth);
          SM_MEMCPY(pz_Dst->f_Elevation,    pz_Src->f_Elevation);
        }
        else
        {
          /*lint -e{736} : Loss of precision (initialization) (56 bits to 32 bits) */
          FLT f_Temp = (-90.0f * ((FLT)C_PI/180.0f));
          /*lint -e{740} : Unusual pointer cast (incompatible indirect types) */
          uint32 *p_Temp = (uint32*) &f_Temp;
          pz_Dst->f_Azimuth               = 0;
          pz_Dst->f_Elevation             = *p_Temp;
        }
        if ((pz_Src->q_MeasStatus & MEAS_STATUS_VE_FINE) &&
            (pz_Src->q_MeasStatus & MEAS_STATUS_VE_VALID))
        {
          SM_MEMCPY(pz_Dst->f_FineSpeed,    pz_Src->f_FineSpeed);
          SM_MEMCPY(pz_Dst->f_FineSpeedUnc, pz_Src->f_FineSpeedUnc);
        }

        SM_MEMCPY(pz_Dst->f_DopplerAccel, pz_Src->f_DopplerAccel);
        SM_MEMCPY(pz_Dst->d_CarrierPhase, pz_Src->d_CarrierPhase);

        pz_Dst->q_FCount = pz_Src->q_FCount;
        pz_Dst->w_ParityErrorCount = pz_Src->w_ParityErrorCount;
        pz_Dst->u_GoodParity = pz_Src->u_GoodParity;
      }
    }
    else if (GNSS_MEAS_BLK_SRC_GLO == p_LogPkt->z_MeasBlk.e_MeasBlkSrc)
    {
      gnss_MeasStructType_ForDmLog* pz_Dst;
      const gnss_MeasStructType*       pz_Src;

      //Fields for glo_TimeStructType
      pz_DstTime->z_GloTime.e_SourceOfTime  = pz_SrcTime->z_GloTime.e_SourceOfTime;
      pz_DstTime->z_GloTime.u_FourYear      = pz_SrcTime->z_GloTime.u_FourYear;
      pz_DstTime->z_GloTime.w_Days          = pz_SrcTime->z_GloTime.w_Days;
      pz_DstTime->z_GloTime.q_GloMsec       = pz_SrcTime->z_GloTime.q_GloMsec;

      SM_MEMCPY(pz_DstTime->z_GloTime.f_ClkTimeBias,
                pz_SrcTime->z_GloTime.f_ClkTimeBias);

      f_data = pz_SrcTime->z_GloTime.f_ClkTimeUncMs * 1.0e6f;
      SM_MEMCPY(pz_DstTime->z_GloTime.f_ClkTimeUncMs, f_data);

      //Fields for gnss_GloMeasStructType
      for(i=0; i < MIN(ARR_SIZE(p_LogPkt->z_MeasBlk.n_Meas.z_Glo),
                       p_measReport->z_MeasBlk.u_NumSvs); i++)
      {
        pz_Dst = &p_LogPkt->z_MeasBlk.n_Meas.z_Glo[i];
        pz_Src = &p_measReport->z_MeasBlk.z_Gnss[i];

        pz_Dst->w_Sv                        = pz_Src->w_Sv; //Legacy OEMDRE does not support GAL 
        pz_Dst->b_FreqNum                   = pz_Src->b_FreqNum;
        pz_Dst->e_SvState                   = pz_Src->q_SvState;
        pz_Dst->u_Observe                   = pz_Src->u_Observe;
        pz_Dst->u_GoodObs                   = pz_Src->u_GoodObs;
        pz_Dst->u_FilterN                   = pz_Src->u_FilterN;
        pz_Dst->u_PreInt                    = pz_Src->u_PreInt;
        pz_Dst->u_CSlipCount                = pz_Src->u_CSlipCount;
        pz_Dst->w_PostInt                   = pz_Src->w_PostInt;
        pz_Dst->w_Cno                       = pz_Src->w_Cno * 10;
        pz_Dst->w_RfLoss                    = pz_Src->w_RfLoss;
        pz_Dst->x_LatencyMs                 = (int16)pz_Src->l_LatencyMs;
        pz_Dst->q_MeasStatus                = pz_Src->q_MeasStatus;

        /* Filtered measurement */
        SM_MEMCPY(pz_Dst->z_FiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_FiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_FiltMeas.z_SvTime.q_Ms    = pz_Src->z_FiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvTimeUncMs,
                  pz_Src->z_FiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeed,
                  pz_Src->z_FiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeedUnc,
                  pz_Src->z_FiltMeas.f_SvSpeedUnc);

        /* Un-filtered measurement */
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_UnfiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_UnfiltMeas.z_SvTime.q_Ms  = pz_Src->z_UnfiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvTimeUncMs,
                  pz_Src->z_UnfiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeed,
                  pz_Src->z_UnfiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeedUnc,
                  pz_Src->z_UnfiltMeas.f_SvSpeedUnc);

        pz_Dst->u_MultipathEstValid         = pz_Src->u_MultipathEstValid;
        if (FALSE != pz_Src->u_MultipathEstValid)
        {
          SM_MEMCPY(pz_Dst->f_MultipathEst, pz_Src->f_MultipathEst);
        }
        pz_Dst->u_SvDirValid = pz_Src->u_SvDirValid;
        if (FALSE != pz_Src->u_SvDirValid)
        {
          SM_MEMCPY(pz_Dst->f_Azimuth,      pz_Src->f_Azimuth);
          SM_MEMCPY(pz_Dst->f_Elevation,    pz_Src->f_Elevation);
        }
        else
        {
          /*lint -e{736} : Loss of precision (initialization) (56 bits to 32 bits) */
          FLT f_Temp = (-90.0f * ((FLT)C_PI/180.0f));
          /*lint -e{740} : Unusual pointer cast (incompatible indirect types) */
          uint32 *p_Temp = (uint32*) &f_Temp;
          pz_Dst->f_Azimuth                 = 0;
          pz_Dst->f_Elevation               = *p_Temp;
        }
        if ((pz_Src->q_MeasStatus & MEAS_STATUS_VE_FINE) &&
            (pz_Src->q_MeasStatus & MEAS_STATUS_VE_VALID))
        {
          SM_MEMCPY(pz_Dst->f_FineSpeed,    pz_Src->f_FineSpeed);
          SM_MEMCPY(pz_Dst->f_FineSpeedUnc, pz_Src->f_FineSpeedUnc);
        }
        SM_MEMCPY(pz_Dst->f_DopplerAccel,   pz_Src->f_DopplerAccel);
        SM_MEMCPY(pz_Dst->d_CarrierPhase,   pz_Src->d_CarrierPhase);

        pz_Dst->q_FCount                    = pz_Src->q_FCount;
        pz_Dst->w_ParityErrorCount = pz_Src->w_ParityErrorCount;
        pz_Dst->u_GoodParity = pz_Src->u_GoodParity;
      }
    }
    else if (GNSS_MEAS_BLK_SRC_BDS == p_LogPkt->z_MeasBlk.e_MeasBlkSrc)
    {
      gnss_MeasStructType_ForDmLog* pz_Dst;
      const gnss_MeasStructType*    pz_Src;

      //Fields for gps_TimeStructType
      pz_DstTime->z_GnssTime.w_GpsWeek        = pz_SrcTime->z_BdsTime.w_BdsWeek;
      pz_DstTime->z_GnssTime.q_GpsMsec        = pz_SrcTime->z_BdsTime.q_BdsMsec;

      SM_MEMCPY(pz_DstTime->z_GnssTime.f_ClkTimeBias,
                pz_SrcTime->z_BdsTime.f_ClkTimeBias);

      f_data = pz_SrcTime->z_BdsTime.f_ClkTimeUncMs * 1.0e6f;
      SM_MEMCPY(pz_DstTime->z_GnssTime.f_ClkTimeUncMs, f_data);

      pz_DstTime->z_GnssTime.e_SourceOfTime   = pz_SrcTime->z_BdsTime.e_SourceOfTime;

      //Fields for gnss_GpsMeasStructType
      for(i=0; i < MIN(ARR_SIZE(p_LogPkt->z_MeasBlk.n_Meas.z_Gps),
                       p_measReport->z_MeasBlk.u_NumSvs); i++)
      {
        pz_Dst = &p_LogPkt->z_MeasBlk.n_Meas.z_Gps[i];
        pz_Src = &p_measReport->z_MeasBlk.z_Gnss[i];

        pz_Dst->w_Sv                      = pz_Src->w_Sv;
        pz_Dst->e_SvState                 = pz_Src->q_SvState;
        pz_Dst->u_Observe                 = pz_Src->u_Observe;
        pz_Dst->u_GoodObs                 = pz_Src->u_GoodObs;
        pz_Dst->u_FilterN                 = pz_Src->u_FilterN;
        pz_Dst->u_PreInt                  = pz_Src->u_PreInt;
        pz_Dst->u_CSlipCount              = pz_Src->u_CSlipCount;
        pz_Dst->w_PostInt                 = pz_Src->w_PostInt;
        pz_Dst->q_MeasStatus              = pz_Src->q_MeasStatus;
        pz_Dst->q_MeasStatus2             = pz_Src->q_MeasStatus2;
        pz_Dst->w_Cno                     = pz_Src->w_Cno * 10;
        pz_Dst->x_LatencyMs               = (int16)pz_Src->l_LatencyMs;

        /* Filtered measurement */
        SM_MEMCPY(pz_Dst->z_FiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_FiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_FiltMeas.z_SvTime.q_Ms  = pz_Src->z_FiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvTimeUncMs,
                  pz_Src->z_FiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeed,
                  pz_Src->z_FiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeedUnc,
                  pz_Src->z_FiltMeas.f_SvSpeedUnc);

        /* Un-filtered measurement */
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_UnfiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_UnfiltMeas.z_SvTime.q_Ms    = pz_Src->z_UnfiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvTimeUncMs,
                  pz_Src->z_UnfiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeed,
                  pz_Src->z_UnfiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeedUnc,
                  pz_Src->z_UnfiltMeas.f_SvSpeedUnc);

        pz_Dst->u_MultipathEstValid       = pz_Src->u_MultipathEstValid;
        if (FALSE != pz_Src->u_MultipathEstValid)
        {
          SM_MEMCPY(pz_Dst->f_MultipathEst, pz_Src->f_MultipathEst);
        }

        pz_Dst->u_SvDirValid              = pz_Src->u_SvDirValid;
        if (FALSE != pz_Src->u_SvDirValid)
        {
          SM_MEMCPY(pz_Dst->f_Azimuth,      pz_Src->f_Azimuth);
          SM_MEMCPY(pz_Dst->f_Elevation,    pz_Src->f_Elevation);
        }
        else
        {
          /*lint -e{736} : Loss of precision (initialization) (56 bits to 32 bits) */
          FLT f_Temp = (-90.0f * ((FLT)C_PI/180.0f));
          /*lint -e{740} : Unusual pointer cast (incompatible indirect types) */
          uint32 *p_Temp = (uint32*) &f_Temp;
          pz_Dst->f_Azimuth               = 0;
          pz_Dst->f_Elevation             = *p_Temp;
        }
        if ((pz_Src->q_MeasStatus & MEAS_STATUS_VE_FINE) &&
            (pz_Src->q_MeasStatus & MEAS_STATUS_VE_VALID))
        {
          SM_MEMCPY(pz_Dst->f_FineSpeed,    pz_Src->f_FineSpeed);
          SM_MEMCPY(pz_Dst->f_FineSpeedUnc, pz_Src->f_FineSpeedUnc);
        }

        SM_MEMCPY(pz_Dst->f_DopplerAccel, pz_Src->f_DopplerAccel);
        SM_MEMCPY(pz_Dst->d_CarrierPhase, pz_Src->d_CarrierPhase);

        pz_Dst->q_FCount = pz_Src->q_FCount;
        pz_Dst->w_ParityErrorCount = pz_Src->w_ParityErrorCount;
        pz_Dst->u_GoodParity = pz_Src->u_GoodParity;
      }
    }
    else if (GNSS_MEAS_BLK_SRC_GAL == p_LogPkt->z_MeasBlk.e_MeasBlkSrc)
    {
      gnss_MeasStructType_ForDmLog* pz_Dst;
      const gnss_MeasStructType*    pz_Src;

      //Fields for gps_TimeStructType
      pz_DstTime->z_GnssTime.w_GpsWeek        = pz_SrcTime->z_GalTime.w_GalWeek;
      pz_DstTime->z_GnssTime.q_GpsMsec        = pz_SrcTime->z_GalTime.q_GalMsec;

      SM_MEMCPY(pz_DstTime->z_GnssTime.f_ClkTimeBias,
                pz_SrcTime->z_GalTime.f_ClkTimeBias);

      f_data = pz_SrcTime->z_GalTime.f_ClkTimeUncMs * 1.0e6f;
      SM_MEMCPY(pz_DstTime->z_GnssTime.f_ClkTimeUncMs, f_data);

      pz_DstTime->z_GnssTime.e_SourceOfTime   = pz_SrcTime->z_GalTime.e_SourceOfTime;

      //Fields for gnss_GpsMeasStructType
      for(i=0; i < MIN(ARR_SIZE(p_LogPkt->z_MeasBlk.n_Meas.z_Gps),
                       p_measReport->z_MeasBlk.u_NumSvs); i++)
      {
        pz_Dst = &p_LogPkt->z_MeasBlk.n_Meas.z_Gps[i];
        pz_Src = &p_measReport->z_MeasBlk.z_Gnss[i];

        pz_Dst->w_Sv                      = pz_Src->w_Sv;
        pz_Dst->e_SvState                 = pz_Src->q_SvState;
        pz_Dst->u_Observe                 = pz_Src->u_Observe;
        pz_Dst->u_GoodObs                 = pz_Src->u_GoodObs;
        pz_Dst->u_FilterN                 = pz_Src->u_FilterN;
        pz_Dst->u_PreInt                  = pz_Src->u_PreInt;
        pz_Dst->u_CSlipCount              = pz_Src->u_CSlipCount;
        pz_Dst->w_PostInt                 = pz_Src->w_PostInt;
        pz_Dst->q_MeasStatus              = pz_Src->q_MeasStatus;
        pz_Dst->q_MeasStatus2             = pz_Src->q_MeasStatus2;
        pz_Dst->w_Cno                     = pz_Src->w_Cno * 10;
        pz_Dst->x_LatencyMs               = (int16)pz_Src->l_LatencyMs;
        /* Filtered measurement */
        SM_MEMCPY(pz_Dst->z_FiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_FiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_FiltMeas.z_SvTime.q_Ms  = pz_Src->z_FiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvTimeUncMs,
                  pz_Src->z_FiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeed,
                  pz_Src->z_FiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_FiltMeas.f_SvSpeedUnc,
                  pz_Src->z_FiltMeas.f_SvSpeedUnc);

        /* Un-filtered measurement */
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.z_SvTime.f_SubMs,
                  pz_Src->z_UnfiltMeas.z_SvTime.f_SubMs);
        pz_Dst->z_UnfiltMeas.z_SvTime.q_Ms    = pz_Src->z_UnfiltMeas.z_SvTime.q_Ms;
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvTimeUncMs,
                  pz_Src->z_UnfiltMeas.f_SvTimeUncMs);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeed,
                  pz_Src->z_UnfiltMeas.f_SvSpeed);
        SM_MEMCPY(pz_Dst->z_UnfiltMeas.f_SvSpeedUnc,
                  pz_Src->z_UnfiltMeas.f_SvSpeedUnc);

        pz_Dst->u_MultipathEstValid       = pz_Src->u_MultipathEstValid;
        if (FALSE != pz_Src->u_MultipathEstValid)
        {
          SM_MEMCPY(pz_Dst->f_MultipathEst, pz_Src->f_MultipathEst);
        }

        pz_Dst->u_SvDirValid              = pz_Src->u_SvDirValid;
        if (FALSE != pz_Src->u_SvDirValid)
        {
          SM_MEMCPY(pz_Dst->f_Azimuth,      pz_Src->f_Azimuth);
          SM_MEMCPY(pz_Dst->f_Elevation,    pz_Src->f_Elevation);
        }
        else
        {
          /*lint -e{736} : Loss of precision (initialization) (56 bits to 32 bits) */
          FLT f_Temp = (-90.0f * ((FLT)C_PI/180.0f));
          /*lint -e{740} : Unusual pointer cast (incompatible indirect types) */
          uint32 *p_Temp = (uint32*) &f_Temp;
          pz_Dst->f_Azimuth               = 0;
          pz_Dst->f_Elevation             = *p_Temp;
        }
        if ((pz_Src->q_MeasStatus & MEAS_STATUS_VE_FINE) &&
            (pz_Src->q_MeasStatus & MEAS_STATUS_VE_VALID))
        {
          SM_MEMCPY(pz_Dst->f_FineSpeed,    pz_Src->f_FineSpeed);
          SM_MEMCPY(pz_Dst->f_FineSpeedUnc, pz_Src->f_FineSpeedUnc);
        }

        SM_MEMCPY(pz_Dst->f_DopplerAccel, pz_Src->f_DopplerAccel);
        SM_MEMCPY(pz_Dst->d_CarrierPhase, pz_Src->d_CarrierPhase);

        pz_Dst->q_FCount = pz_Src->q_FCount;
        pz_Dst->w_ParityErrorCount = pz_Src->w_ParityErrorCount;
        pz_Dst->u_GoodParity = pz_Src->u_GoodParity;
      }
    }
    log_commit(p_LogPkt);
  }
  return;
}

/*
 ******************************************************************************
 * sm_log_oemdre_position_fix_report
 *
 * Function description:
 *
 * This function generates a DM log message when position fix report is sent to the
 * OEM
 *
 * Parameters:
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_oemdre_position_fix_report(const sm_GnssFixRptStructType* p_fixReport)
{
  LOG_GNSS_OEMDRE_POSITION_REPORT_C_type* p_LogPkt;
  sm_GnssNavSolution_ForDmLog*            pz_DstNavSol;
  sm_GnssNavSolutionExtra_ForDmLog*       pz_DstNavSolExtra;
  sm_SensorParamRptStructType_ForDmLog*    pz_DstOnDeviceSensorParam;
  const sm_GnssNavPosition*               pz_SrcNavSol;
  const sm_GnssNavSvInfo*          pz_SrcNavSolExtra;
  const sm_SensorParamRptStructType* pz_SrcOnDeviceSensorParam; /* sensor-related info */

  uint8 i=0, counter = 0;

  if (p_fixReport == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet LOG_GNSS_OEMDRE_POSITION_REPORT_C*/
  p_LogPkt = (LOG_GNSS_OEMDRE_POSITION_REPORT_C_type*)log_alloc( LOG_GNSS_OEMDRE_POSITION_REPORT_C,
    sizeof(LOG_GNSS_OEMDRE_POSITION_REPORT_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion                = GNSS_GEN8_LOG_VERSION_OEMDRE_V2;

    p_LogPkt->b_nofix                        = FALSE; //Fix report

    // Fields for sm_GnssNavSolution
    SM_MEMCLR(p_LogPkt->z_NavSolution);
    pz_DstNavSol = &p_LogPkt->z_NavSolution;
    pz_SrcNavSol = &p_fixReport->z_NavPos;

    //pz_DstNavSol->w_PositionFlags            = (uint16)pz_SrcNavSol->z_SmPosFlags;
    pz_DstNavSol->w_FixWn                    = pz_SrcNavSol->w_GpsWeek;
    pz_DstNavSol->q_FixTime                  = pz_SrcNavSol->q_GpsTimeMs;
    pz_DstNavSol->q_UtcFixTime               = pz_SrcNavSol->q_UtcTimeMs;
    pz_DstNavSol->u_GloNumFourYear           = pz_SrcNavSol->u_GloNumFourYear;
    pz_DstNavSol->w_GloNumDaysInFourYear     = pz_SrcNavSol->w_GloNumDaysInFourYear;
    pz_DstNavSol->q_GloTimeMs                = pz_SrcNavSol->q_GloTimeMs;
    pz_DstNavSol->q_RefFCount                = pz_SrcNavSol->q_RefFCount;
    pz_DstNavSol->u_InitGpsTimeSrc           = pz_SrcNavSol->u_InitGpsTimeSrc;
    pz_DstNavSol->q_PosCounter               = pz_SrcNavSol->q_PosCounter;

    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->d_PosEcef),
                     ARR_SIZE(pz_SrcNavSol->d_PosEcef)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->d_PosEcef[i], pz_SrcNavSol->d_PosEcef[i]);
    }
    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->d_PosLla),
                     ARR_SIZE(pz_SrcNavSol->d_PosLla)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->d_PosLla[i], pz_SrcNavSol->d_PosLla[i]);
    }
    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->d_VelEcef),
                     ARR_SIZE(pz_SrcNavSol->d_VelEcef)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->d_VelEcef[i], pz_SrcNavSol->d_VelEcef[i]);
    }
    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->f_VelEnu),
                     ARR_SIZE(pz_SrcNavSol->f_VelEnu)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->f_VelEnu[i], pz_SrcNavSol->f_VelEnu[i]);
    }

    SM_MEMCPY(pz_DstNavSol->f_HeadingRad, pz_SrcNavSol->f_HeadingRad);
    SM_MEMCPY(pz_DstNavSol->f_HeadingUncRad, pz_SrcNavSol->f_HeadingUncRad);
    SM_MEMCPY(pz_DstNavSol->f_GnssHeadingRad, pz_SrcNavSol->f_GnssHeadingRad);
    SM_MEMCPY(pz_DstNavSol->f_GnssHeadingUncRad, pz_SrcNavSol->f_GnssHeadingUncRad);
    SM_MEMCPY(pz_DstNavSol->f_PosSigma, pz_SrcNavSol->f_PuncLatLonMeters);

    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->f_PosUnc2D),
                     ARR_SIZE(pz_SrcNavSol->f_PuncLatLonMeters)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->f_PosUnc2D[i], pz_SrcNavSol->f_PuncLatLonMeters[i]);
    }

    SM_MEMCPY(pz_DstNavSol->f_PosUncVertical, pz_SrcNavSol->f_PuncVertMeters);
    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->f_ErrorEllipse),
                     ARR_SIZE(pz_SrcNavSol->f_ErrorEllipse)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->f_ErrorEllipse[i], pz_SrcNavSol->f_ErrorEllipse[i]);
    }
    
    SM_MEMCPY(pz_DstNavSol->f_VelUnc, pz_SrcNavSol->f_Vunc3dMps);

    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->f_VuncEastNorthMps),
                     ARR_SIZE(pz_SrcNavSol->f_VuncEastNorthMps)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->f_VuncEastNorthMps[i], 
                pz_SrcNavSol->f_VuncEastNorthMps[i]);
    }
    SM_MEMCPY(pz_DstNavSol->f_VuncVertMps, pz_SrcNavSol->f_VuncVertMps);
    
    SM_MEMCPY(pz_DstNavSol->f_ClockBias, pz_SrcNavSol->f_ClockBias);
    SM_MEMCPY(pz_DstNavSol->f_ClockBiasUncMs, pz_SrcNavSol->f_ClockBiasUncMs);
    SM_MEMCPY(pz_DstNavSol->f_ClockDriftRate, pz_SrcNavSol->f_ClockDriftRate);
    SM_MEMCPY(pz_DstNavSol->f_ClockDriftRateUncMps,pz_SrcNavSol->f_ClockDriftRateUncMps);

    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSol->z_InterGnssTb),
      ARR_SIZE(pz_SrcNavSol->z_InterGnssTb)); i++)
    {
      SM_MEMCPY(pz_DstNavSol->z_InterGnssTb[i].f_UnfiltMeters, pz_SrcNavSol->z_InterGnssTb[i].f_UnfiltMeters);
      SM_MEMCPY(pz_DstNavSol->z_InterGnssTb[i].f_UnfiltUncMeters, pz_SrcNavSol->z_InterGnssTb[i].f_UnfiltUncMeters);
      SM_MEMCPY(pz_DstNavSol->z_InterGnssTb[i].q_FiltAgeMs, pz_SrcNavSol->z_InterGnssTb[i].q_FiltAgeMs);
      SM_MEMCPY(pz_DstNavSol->z_InterGnssTb[i].f_FiltMeters, pz_SrcNavSol->z_InterGnssTb[i].f_FiltMeters);
      SM_MEMCPY(pz_DstNavSol->z_InterGnssTb[i].f_FiltUncMeters, pz_SrcNavSol->z_InterGnssTb[i].f_FiltUncMeters);
      SM_MEMCPY(pz_DstNavSol->z_InterGnssTb[i].b_IsPeTbConfident, pz_SrcNavSol->z_InterGnssTb[i].b_IsPeTbConfident);
    }

    SM_MEMCPY(pz_DstNavSol->f_FilteredAlt, pz_SrcNavSol->f_FilteredAlt);
    SM_MEMCPY(pz_DstNavSol->f_FilteredAltUnc, pz_SrcNavSol->f_FilteredAltUnc);
    SM_MEMCPY(pz_DstNavSol->f_UnfilteredAlt, pz_SrcNavSol->f_UnfilteredAlt);
    SM_MEMCPY(pz_DstNavSol->f_UnfilteredAltUnc, pz_SrcNavSol->f_UnfilteredAltUnc);
    SM_MEMCPY(pz_DstNavSol->f_MagDeviation, pz_SrcNavSol->f_MagDeviation);
    SM_MEMCPY(pz_DstNavSol->f_PDOP, pz_SrcNavSol->f_PDOP);
    SM_MEMCPY(pz_DstNavSol->f_HDOP, pz_SrcNavSol->f_HDOP);
    SM_MEMCPY(pz_DstNavSol->f_VDOP, pz_SrcNavSol->f_VDOP);

    pz_DstNavSol->u_FirstFix                 = pz_SrcNavSol->u_FirstFix;
    pz_DstNavSol->u_DriftEstimated           = pz_SrcNavSol->u_DriftEstimated;
    pz_DstNavSol->u_MagDevGood               = pz_SrcNavSol->u_MagDevGood;
    pz_DstNavSol->u_IonoCorrected            = pz_SrcNavSol->u_IonoCorrected;
    pz_DstNavSol->u_TropoCorrected           = pz_SrcNavSol->u_TropoCorrected;
    pz_DstNavSol->e_HoriRelIndicator            = pz_SrcNavSol->e_HoriRelIndicator;
    pz_DstNavSol->e_VertRelIndicator           = pz_SrcNavSol->e_VertRelIndicator;
 
    SM_MEMCPY(pz_DstNavSol->w_SensorDataUsageMask, pz_SrcNavSol->w_SensorDataUsageMask);
    SM_MEMCPY(pz_DstNavSol->w_SensorAidingIndicator, pz_SrcNavSol->w_SensorAidingIndicator);

    // Fields for sm_GnssNavSolutionExtra
    SM_MEMCLR(p_LogPkt->z_NavSolutionExtra);
    pz_DstNavSolExtra = &p_LogPkt->z_NavSolutionExtra;
    pz_SrcNavSolExtra = &p_fixReport->z_NavSvInfo;
    
    pz_DstNavSolExtra->q_RefFCount  = p_fixReport->z_NavPos.q_RefFCount;
    for(i=0; i < MIN(ARR_SIZE(pz_DstNavSolExtra->f_ErrorEllipse),
                     ARR_SIZE(p_fixReport->z_NavPos.f_ErrorEllipse)); i++)
    {
      SM_MEMCPY(pz_DstNavSolExtra->f_ErrorEllipse[i],
                p_fixReport->z_NavPos.f_ErrorEllipse[i]);
    }

    SM_MEMCPY(pz_DstNavSolExtra->f_FilteredAlt,
              p_fixReport->z_NavPos.f_FilteredAlt);
    SM_MEMCPY(pz_DstNavSolExtra->f_FilteredAltUnc,
              p_fixReport->z_NavPos.f_FilteredAltUnc);
    SM_MEMCPY(pz_DstNavSolExtra->f_UnfilteredAlt,
              p_fixReport->z_NavPos.f_UnfilteredAlt);
    SM_MEMCPY(pz_DstNavSolExtra->f_UnfilteredAltUnc,
              p_fixReport->z_NavPos.f_UnfilteredAltUnc);

    // log all GPS SVs used in pos fix and related inforamtion
    pz_DstNavSolExtra->u_nSVsDetected        = p_fixReport->z_NavSvInfo.u_GpsNumSvMeas;
    pz_DstNavSolExtra->u_nSVs                = p_fixReport->z_NavSvInfo.u_GpsNumSvPosFix;
    pz_DstNavSolExtra->u_GpsNumSvVelFix      = p_fixReport->z_NavSvInfo.u_GpsNumSvVelFix;
    pz_DstNavSolExtra->q_GpsSvMaskUsed       = p_fixReport->z_NavSvInfo.q_GpsSvMaskUsed;
    pz_DstNavSolExtra->q_GpsSvMaskUsable     = p_fixReport->z_NavSvInfo.q_GpsSvMaskUsable;

    //pz_DstNavSol->u_nSVs = p_fixReport->z_NavSvInfo.u_GpsNumSvPosFix;
   for (i = 0; i < ARR_SIZE(p_fixReport->z_NavSvInfo.z_GpsSvInfo); i++)
    {
      if ((p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX))
      {
        // This GPS SV used in pos fix
          pz_DstNavSolExtra->z_GpsSvInfo[i].w_SvId = (uint16)p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].w_SvId; //Legacy OEMDRE does not Support GAL
          SM_MEMCPY(pz_DstNavSolExtra->z_GpsSvInfo[i].f_PrResMeters, p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].f_PrResMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_GpsSvInfo[i].f_PrUncMeters, p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].f_PrUncMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_GpsSvInfo[i].f_PrrResMps, p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].f_PrrResMps);
          SM_MEMCPY(pz_DstNavSolExtra->z_GpsSvInfo[i].f_PrrUncMps, p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].f_PrrUncMps);
          pz_DstNavSolExtra->z_GpsSvInfo[i].w_IODE = (uint16)p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].w_IODE;
          pz_DstNavSolExtra->z_GpsSvInfo[i].b_FreqNum = (uint8)p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].b_FreqNum; 
          pz_DstNavSolExtra->z_GpsSvInfo[i].u_SvFSAvail = (uint8)p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].u_SvFSAvail; 
          pz_DstNavSolExtra->z_GpsSvInfo[i].u_SvPrtyGood = (uint8)p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].u_SvPrtyGood; 
          pz_DstNavSolExtra->z_GpsSvInfo[i].w_SvPrtyErr = (uint16)p_fixReport->z_NavSvInfo.z_GpsSvInfo[i].w_SvPrtyErr;

        
      } /* if bitmask */
    } /* for i */

   
    // log all GLO SVs used in pos fix and related inforamtion
    pz_DstNavSolExtra->u_nGloSVsDetected        = p_fixReport->z_NavSvInfo.u_GloNumSvMeas;
    pz_DstNavSolExtra->u_NumGloSvs              = p_fixReport->z_NavSvInfo.u_GloNumSvPosFix;
    pz_DstNavSolExtra->u_GloNumSvVelFix         = p_fixReport->z_NavSvInfo.u_GloNumSvVelFix;
    pz_DstNavSolExtra->q_GloSvMaskUsed          = p_fixReport->z_NavSvInfo.q_GloSvMaskUsed;
    pz_DstNavSolExtra->q_GloSvMaskUsable        = p_fixReport->z_NavSvInfo.q_GloSvMaskUsable;

    //pz_DstNavSol->u_nSVs = p_fixReport->z_NavSvInfo.u_GpsNumSvPosFix;
   for (i = 0; i < ARR_SIZE(p_fixReport->z_NavSvInfo.z_GloSvInfo); i++)
    {
      if ((p_fixReport->z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX))
      {
        // This GLO SV used in pos fix
          pz_DstNavSolExtra->z_GloSvInfo[i].w_SvId = (uint16)p_fixReport->z_NavSvInfo.z_GloSvInfo[i].w_SvId; //Legacy OEMDRE does not Support GAL
          SM_MEMCPY(pz_DstNavSolExtra->z_GloSvInfo[i].f_PrResMeters, p_fixReport->z_NavSvInfo.z_GloSvInfo[i].f_PrResMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_GloSvInfo[i].f_PrUncMeters, p_fixReport->z_NavSvInfo.z_GloSvInfo[i].f_PrUncMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_GloSvInfo[i].f_PrrResMps, p_fixReport->z_NavSvInfo.z_GloSvInfo[i].f_PrrResMps);
          SM_MEMCPY(pz_DstNavSolExtra->z_GloSvInfo[i].f_PrrUncMps, p_fixReport->z_NavSvInfo.z_GloSvInfo[i].f_PrrUncMps);
          pz_DstNavSolExtra->z_GloSvInfo[i].u_GloTb = (uint8)p_fixReport->z_NavSvInfo.z_GloSvInfo[i].u_GloTb;
          pz_DstNavSolExtra->z_GloSvInfo[i].b_FreqNum = (uint8)p_fixReport->z_NavSvInfo.z_GloSvInfo[i].b_FreqNum; 
          pz_DstNavSolExtra->z_GloSvInfo[i].u_SvFSAvail = (uint8)p_fixReport->z_NavSvInfo.z_GloSvInfo[i].u_SvFSAvail; 
          pz_DstNavSolExtra->z_GloSvInfo[i].u_SvPrtyGood = (uint8)p_fixReport->z_NavSvInfo.z_GloSvInfo[i].u_SvPrtyGood; 
          pz_DstNavSolExtra->z_GloSvInfo[i].w_SvPrtyErr = (uint16)p_fixReport->z_NavSvInfo.z_GloSvInfo[i].w_SvPrtyErr;

        
      } /* if bitmask */
    } /* for i */

    // log all BDS SVs used in pos fix and related inforamtion
    pz_DstNavSolExtra->u_nBdsSVsDetected        = p_fixReport->z_NavSvInfo.u_BdsNumSvMeas;
    pz_DstNavSolExtra->u_NumBdsSvs              = p_fixReport->z_NavSvInfo.u_BdsNumSvPosFix;
    pz_DstNavSolExtra->u_BdsNumSvVelFix         = p_fixReport->z_NavSvInfo.u_BdsNumSvVelFix;
    pz_DstNavSolExtra->t_BdsSvMaskUsed          = p_fixReport->z_NavSvInfo.t_BdsSvMaskUsed;
    pz_DstNavSolExtra->t_BdsSvMaskUsable        = p_fixReport->z_NavSvInfo.t_BdsSvMaskUsable;

    //pz_DstNavSol->u_nSVs = p_fixReport->z_NavSvInfo.u_GpsNumSvPosFix;
   for (i = 0; i < ARR_SIZE(p_fixReport->z_NavSvInfo.z_BdsSvInfo); i++)
    {
      if ((p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX))
      {
        // This BDS SV used in pos fix
          pz_DstNavSolExtra->z_BdsSvInfo[i].w_SvId = (uint16)p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].w_SvId; //Legacy OEMDRE does not Support GAL
          SM_MEMCPY(pz_DstNavSolExtra->z_BdsSvInfo[i].f_PrResMeters, p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].f_PrResMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_BdsSvInfo[i].f_PrUncMeters, p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].f_PrUncMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_BdsSvInfo[i].f_PrrResMps, p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].f_PrrResMps);
          SM_MEMCPY(pz_DstNavSolExtra->z_BdsSvInfo[i].f_PrrUncMps, p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].f_PrrUncMps);
          pz_DstNavSolExtra->z_BdsSvInfo[i].w_IODE = (uint16)p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].w_IODE;
          pz_DstNavSolExtra->z_BdsSvInfo[i].b_FreqNum = (uint8)p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].b_FreqNum; 
          pz_DstNavSolExtra->z_BdsSvInfo[i].u_SvFSAvail = (uint8)p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].u_SvFSAvail; 
          pz_DstNavSolExtra->z_BdsSvInfo[i].u_SvPrtyGood = (uint8)p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].u_SvPrtyGood; 
          pz_DstNavSolExtra->z_BdsSvInfo[i].w_SvPrtyErr = (uint16)p_fixReport->z_NavSvInfo.z_BdsSvInfo[i].w_SvPrtyErr;

        
      } /* if bitmask */
    } /* for i */

    // log all GAL SVs used in pos fix and related inforamtion
    pz_DstNavSolExtra->u_nGalSVsDetected        = p_fixReport->z_NavSvInfo.u_GalNumSvMeas;
    pz_DstNavSolExtra->u_NumGalSvs              = p_fixReport->z_NavSvInfo.u_GalNumSvPosFix;
    pz_DstNavSolExtra->u_GalNumSvVelFix         = p_fixReport->z_NavSvInfo.u_GalNumSvVelFix;
    pz_DstNavSolExtra->t_GalSvMaskUsed          = p_fixReport->z_NavSvInfo.t_GalSvMaskUsed;
    pz_DstNavSolExtra->t_GalSvMaskUsable        = p_fixReport->z_NavSvInfo.t_GalSvMaskUsable;

    //pz_DstNavSol->u_nSVs = p_fixReport->z_NavSvInfo.u_GpsNumSvPosFix;
   for (i = 0; i < ARR_SIZE(p_fixReport->z_NavSvInfo.z_GalSvInfo); i++)
    {
      if ((p_fixReport->z_NavSvInfo.z_GalSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX))
      {
        // This GAl SV used in pos fix
          pz_DstNavSolExtra->z_GalSvInfo[i].w_SvId = (uint16)p_fixReport->z_NavSvInfo.z_GalSvInfo[i].w_SvId; //Legacy OEMDRE does not Support GAL
          SM_MEMCPY(pz_DstNavSolExtra->z_GalSvInfo[i].f_PrResMeters, p_fixReport->z_NavSvInfo.z_GalSvInfo[i].f_PrResMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_GalSvInfo[i].f_PrUncMeters, p_fixReport->z_NavSvInfo.z_GalSvInfo[i].f_PrUncMeters);
          SM_MEMCPY(pz_DstNavSolExtra->z_GalSvInfo[i].f_PrrResMps, p_fixReport->z_NavSvInfo.z_GalSvInfo[i].f_PrrResMps);
          SM_MEMCPY(pz_DstNavSolExtra->z_GalSvInfo[i].f_PrrUncMps, p_fixReport->z_NavSvInfo.z_GalSvInfo[i].f_PrrUncMps);
          pz_DstNavSolExtra->z_GalSvInfo[i].w_IODE = (uint16)p_fixReport->z_NavSvInfo.z_GalSvInfo[i].w_IODE;
          pz_DstNavSolExtra->z_GalSvInfo[i].b_FreqNum = (uint8)p_fixReport->z_NavSvInfo.z_GalSvInfo[i].b_FreqNum; 
          pz_DstNavSolExtra->z_GalSvInfo[i].u_SvFSAvail = (uint8)p_fixReport->z_NavSvInfo.z_GalSvInfo[i].u_SvFSAvail; 
          pz_DstNavSolExtra->z_GalSvInfo[i].u_SvPrtyGood = (uint8)p_fixReport->z_NavSvInfo.z_GalSvInfo[i].u_SvPrtyGood; 
          pz_DstNavSolExtra->z_GalSvInfo[i].w_SvPrtyErr = (uint16)p_fixReport->z_NavSvInfo.z_GalSvInfo[i].w_SvPrtyErr;

        
      } /* if bitmask */
    } /* for i */

   SM_MEMCLR(p_LogPkt->z_OnDeviceSensorParam);
    pz_DstOnDeviceSensorParam = &p_LogPkt->z_OnDeviceSensorParam;
    pz_SrcOnDeviceSensorParam = &p_fixReport->z_OnDeviceSensorParam;
   
    pz_DstOnDeviceSensorParam->w_SeqenceNumber  = pz_SrcOnDeviceSensorParam->w_SeqenceNumber;
    pz_DstOnDeviceSensorParam->z_GyroBias.w_SensorValidMask  = pz_SrcOnDeviceSensorParam->z_GyroBias.w_SensorValidMask;
    if (pz_SrcOnDeviceSensorParam->z_GyroBias.w_SensorValidMask & SENSOR_BIAS_VALID_XAXIS)
    {
      SM_MEMCPY(pz_DstOnDeviceSensorParam->z_GyroBias.f_XaxisValue, pz_SrcOnDeviceSensorParam->z_GyroBias.f_XaxisValue);
    }
    if (pz_SrcOnDeviceSensorParam->z_GyroBias.w_SensorValidMask & SENSOR_BIAS_VALID_YAXIS)
    {
      SM_MEMCPY(pz_DstOnDeviceSensorParam->z_GyroBias.f_XaxisValue, pz_SrcOnDeviceSensorParam->z_GyroBias.f_XaxisValue);
    }
  
    if (pz_SrcOnDeviceSensorParam->z_GyroBias.w_SensorValidMask & SENSOR_BIAS_VALID_ZAXIS)
    {
      SM_MEMCPY(pz_DstOnDeviceSensorParam->z_GyroBias.f_XaxisValue, pz_SrcOnDeviceSensorParam->z_GyroBias.f_XaxisValue);
    }
    pz_DstOnDeviceSensorParam->z_AccelBias.w_SensorValidMask  = pz_SrcOnDeviceSensorParam->z_AccelBias.w_SensorValidMask;
    if (pz_SrcOnDeviceSensorParam->z_AccelBias.w_SensorValidMask & SENSOR_BIAS_VALID_XAXIS)
    {
      SM_MEMCPY(pz_DstOnDeviceSensorParam->z_AccelBias.f_XaxisValue, pz_SrcOnDeviceSensorParam->z_AccelBias.f_XaxisValue);
    }
    if (pz_SrcOnDeviceSensorParam->z_AccelBias.w_SensorValidMask & SENSOR_BIAS_VALID_YAXIS)
    {
      SM_MEMCPY(pz_DstOnDeviceSensorParam->z_AccelBias.f_XaxisValue, pz_SrcOnDeviceSensorParam->z_AccelBias.f_XaxisValue);
    }
   
    if (pz_SrcOnDeviceSensorParam->z_AccelBias.w_SensorValidMask & SENSOR_BIAS_VALID_ZAXIS)
   {
     SM_MEMCPY(pz_DstOnDeviceSensorParam->z_AccelBias.f_XaxisValue, pz_SrcOnDeviceSensorParam->z_AccelBias.f_XaxisValue);
   }

    // Fields for No fix report, using the b_nofix flag on top of structure
    p_LogPkt->w_PositionFlags                = 0;
    p_LogPkt->q_FailureCode                  = 0;
    p_LogPkt->u_GpsTimeIsValid               = 0;
    p_LogPkt->w_FixWn                        = 0;
    p_LogPkt->q_FixTime                      = 0;
    p_LogPkt->q_UtcFixTime                   = 0;
    p_LogPkt->q_RefFCount                    = 0;
    p_LogPkt->w_PositionFlagsEx              = 0;
    p_LogPkt->w_Reserved                     = 0;

    log_commit(p_LogPkt);
  }
  return;
}

/*
 ******************************************************************************
 * sm_log_oemdre_sv_poly_report
 *
 * Function description:
 *
 * This function generates a DM log message when sv poly report is sent to the
 * OEM
 *
 * Parameters:
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_oemdre_sv_poly_report(const gnss_SvPoly* p_svPolyReport)
{
  LOG_GNSS_OEMDRE_SVPOLY_REPORT_C_type* p_LogPkt;
  uint8 i=0;

  if (p_svPolyReport == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet LOG_GNSS_OEMDRE_SVPOLY_REPORT_C*/
  p_LogPkt = (LOG_GNSS_OEMDRE_SVPOLY_REPORT_C_type*)log_alloc( LOG_GNSS_OEMDRE_SVPOLY_REPORT_C,
    sizeof(LOG_GNSS_OEMDRE_SVPOLY_REPORT_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion  = GNSS_GEN8_LOG_VERSION_OEMDRE_V2;

    p_LogPkt->w_Sv             = p_svPolyReport->w_SvId;
    p_LogPkt->b_FreqNum        = p_svPolyReport->b_FreqNum;
    p_LogPkt->u_SvPolyFlags    = p_svPolyReport->u_SvPolyFlags;
    p_LogPkt->w_Iode           = (uint8)p_svPolyReport->w_Iode;

    SM_MEMCPY(p_LogPkt->d_T0, p_svPolyReport->d_T0);

    for(i=0; i < MIN(ARR_SIZE(p_LogPkt->pd_xyz0), 
                     ARR_SIZE(p_svPolyReport->pd_xyz0)); i++)
    {
      SM_MEMCPY(p_LogPkt->pd_xyz0[i], p_svPolyReport->pd_xyz0[i]);
    }
    for(i=0; i < MIN(ARR_SIZE(p_LogPkt->pd_xyzN), 
                     ARR_SIZE(p_svPolyReport->pd_xyzN)); i++)
    {
      SM_MEMCPY(p_LogPkt->pd_xyzN[i], p_svPolyReport->pd_xyzN[i]);
    }
    for(i=0; i < MIN(ARR_SIZE(p_LogPkt->pf_other), 
                     ARR_SIZE(p_svPolyReport->pf_other)); i++)
    {
      SM_MEMCPY(p_LogPkt->pf_other[i], p_svPolyReport->pf_other[i]);
    }

    SM_MEMCPY(p_LogPkt->f_PosUnc,        p_svPolyReport->f_PosUnc);
    SM_MEMCPY(p_LogPkt->f_IonoDelay,     p_svPolyReport->f_IonoDelay);
    SM_MEMCPY(p_LogPkt->f_IonoDot,       p_svPolyReport->f_IonoDot);
    SM_MEMCPY(p_LogPkt->f_sbasIonoDelay, p_svPolyReport->f_sbasIonoDelay);
    SM_MEMCPY(p_LogPkt->f_sbasIonoDot,   p_svPolyReport->f_sbasIonoDot);
    SM_MEMCPY(p_LogPkt->f_TropoDelay,    p_svPolyReport->f_TropoDelay);
    SM_MEMCPY(p_LogPkt->f_Elevation,     p_svPolyReport->f_Elevation);
    SM_MEMCPY(p_LogPkt->f_ElevDot,       p_svPolyReport->f_ElevDot);
    SM_MEMCPY(p_LogPkt->f_ElevUnc,       p_svPolyReport->f_ElevUnc);

    for(i=0; i < MIN(ARR_SIZE(p_LogPkt->d_VelCoef), 
                     ARR_SIZE(p_svPolyReport->d_VelCoef)); i++)
    {
      SM_MEMCPY(p_LogPkt->d_VelCoef[i], p_svPolyReport->d_VelCoef[i]);
    }

    log_commit(p_LogPkt);
  }
  return;
}

/*
******************************************************************************
* sm_log_oemdrsync_pulse_desc
*
* Function description:
*
* This function generates a DM log message for DrSync pulse description
*
* Parameters:
* Return value:
*
*  None
*
******************************************************************************
*/
void sm_log_oemdrsync_pulse_desc(const gnss_DRSyncPulseDescType *pOemDrSyncPulseDesc)
{
  LOG_GNSS_OEMDRSYNC_C_type *p_LogPkt;

  if (NULL == pOemDrSyncPulseDesc){
    return;
  }

  /* Try to allocate memory for the log packet LOG_GNSS_OEMDRSYNC_C*/
  p_LogPkt = (LOG_GNSS_OEMDRSYNC_C_type*)log_alloc( LOG_GNSS_OEMDRSYNC_C,
    sizeof(LOG_GNSS_OEMDRSYNC_C_type));

  if (p_LogPkt) {
    p_LogPkt->u_PacketVersion   = GNSS_GEN8_LOG_VERSION;
    p_LogPkt->u_DRSyncLogType   = LOG_GNSS_OEMDRSYNC_C_PULSE_DESC;
    
    p_LogPkt->w_GpsWeek         = pOemDrSyncPulseDesc->w_GpsWeek;
    p_LogPkt->q_Count           = pOemDrSyncPulseDesc->q_Count;

    SM_MEMCPY(p_LogPkt->f_GpsMsec, pOemDrSyncPulseDesc->f_GpsMsec);
    SM_MEMCPY(p_LogPkt->f_ClkTimeUncMs, pOemDrSyncPulseDesc->f_ClkTimeUncMs);

    log_commit(p_LogPkt);
  }

  return;
}

/*
******************************************************************************
* sm_log_oemdrsync_config
*
* Function description:
*
* This function generates a DM log message for DRSync configuration parameters
*
* Parameters:
* Return value:
*
*  None
*
******************************************************************************
*/
void sm_log_oemdrsync_config(const gnss_DRSyncConfigType* pOemDrSyncConfig)
{
  LOG_GNSS_OEMDRSYNC_C_type *p_LogPkt;

  if (NULL == pOemDrSyncConfig){
    return;
  }

  /* Try to allocate memory for the log packet LOG_GNSS_OEMDRSYNC_C*/
  p_LogPkt = (LOG_GNSS_OEMDRSYNC_C_type*)log_alloc( LOG_GNSS_OEMDRSYNC_C,
    sizeof(LOG_GNSS_OEMDRSYNC_C_type));

  if (p_LogPkt) {
    p_LogPkt->u_PacketVersion  = GNSS_GEN8_LOG_VERSION;
    p_LogPkt->u_DRSyncLogType  = LOG_GNSS_OEMDRSYNC_C_CONFIG;

    p_LogPkt->e_Mode           = pOemDrSyncConfig->e_Mode;
    p_LogPkt->e_Polarity       = pOemDrSyncConfig->e_Polarity;
    p_LogPkt->l_PhaseSec       = pOemDrSyncConfig->l_PhaseSec;
    p_LogPkt->l_PeriodSec      = pOemDrSyncConfig->l_PeriodSec;
    p_LogPkt->b_TuncIgnore     = pOemDrSyncConfig->b_TuncIgnore;
    p_LogPkt->b_Armed          = pOemDrSyncConfig->b_Armed;

    SM_MEMCPY(p_LogPkt->f_BiasNs, pOemDrSyncConfig->f_BiasNs);

    SM_MEMCPY(p_LogPkt->f_TuncStartThresholdMs, pOemDrSyncConfig->f_TuncStartThresholdMs);

    SM_MEMCPY(p_LogPkt->f_TuncStopThresholdMs, pOemDrSyncConfig->f_TuncStopThresholdMs);

    log_commit(p_LogPkt);

  }

  return;
}

/*
******************************************************************************
* sm_log_ftcal_data
*
* Function description:
*
* This function generates a DM log message for generated Fast TCal data
*
* Parameters:  
* FTCal PPM data (p_Ppm)
* FTCal TT Data  (p_TT)
* FTCal GPS data (p_Gps)
* FTCal TCal Value (p_TCal)
* 
* Return value:
* TRUE if logging was successful
* FALSE if logging failed
*
******************************************************************************
*/
boolean sm_log_ftcal_data
(
  const cgps_FtcalPpmStruct* p_Ppm,
  const cgps_FtcalTTStruct*  p_TT,
  const cgps_FtcalGpsStruct* p_Gps,
  const cgps_FtcalStruct*    p_TCal
)
{
  log_ftcal_type * p_FTCalLog = NULL;
   
  if (( p_Ppm == NULL ) || ( p_TT == NULL) || 
      ( p_Gps == NULL ) || ( p_TCal == NULL))
  {
    return FALSE;
  }
  p_FTCalLog = (log_ftcal_type *)log_alloc( LOG_GNSS_FAST_TCAL_C, sizeof(log_ftcal_type) );

  if (p_FTCalLog != NULL)
  {
    p_FTCalLog->q_Version = LOG_GNSS_FAST_TCAL_VERSION;

    p_FTCalLog->u_ErrorCode           = p_TCal->u_ErrorCode;
    p_FTCalLog->f_TCalAverValueNsec   = p_TCal->f_TCalAverValueNsec;
    p_FTCalLog->q_TCalUncertaintyNsec = p_TCal->q_TCalUncertaintyNsec;
    p_FTCalLog->f_TCalMaxValueNsec    = p_TCal->f_TCalMaxValueNsec;
    p_FTCalLog->f_TCalMinValueNsec    = p_TCal->f_TCalMinValueNsec;

    p_FTCalLog->f_PpmAverToaCCx16             = p_Ppm->f_PpmAverToaCCx16;
    p_FTCalLog->l_PpmSrchWinCenterCCx16       = p_Ppm->l_PpmSrchWinCenterCCx16;
    p_FTCalLog->q_PpmNumToaSamplesAccumulated = p_Ppm->q_PpmNumToaSamplesAccumulated;
    p_FTCalLog->x_PpmMaxToaCCx16              = p_Ppm->x_PpmMaxToaCCx16;
    p_FTCalLog->x_PpmMinToaCCx16              = p_Ppm->x_PpmMinToaCCx16;
    p_FTCalLog->f_PpmAverEcIodB               = p_Ppm->f_PpmAverEcIodB;
    p_FTCalLog->f_PpmMaxEcIodB                = p_Ppm->f_PpmMaxEcIodB;
    p_FTCalLog->f_PpmMinEcIodB                = p_Ppm->f_PpmMinEcIodB;

    p_FTCalLog->f_TtGnssRtcS80SubMsecGCx80      = p_TT->f_TtGnssRtcS80SubMsecGCx80;
    p_FTCalLog->f_TtRefCdmaRtcS80SubPnRollCCx16 = p_TT->f_TtRefCdmaRtcS80SubPnRollCCx16;
    p_FTCalLog->q_TtRefGnssRtcs80FCount         = p_TT->q_TtRefGnssRtcs80FCount;
    p_FTCalLog->q_TtNumTTSamplesAccumulated     = p_TT->q_TtNumTTSamplesAccumulated;
    p_FTCalLog->f_TtMaxGnssRtcS80SubMsecGCx80   = p_TT->f_TtMaxGnssRtcS80SubMsecGCx80;
    p_FTCalLog->f_TtMinGnssRtcS80SubMsecGCx80   = p_TT->f_TtMinGnssRtcS80SubMsecGCx80;

    p_FTCalLog->u_GpsSv                = p_Gps->u_GpsSv;
    p_FTCalLog->f_GpsPeakCodePhSubMsec = p_Gps->f_GpsPeakCodePhSubMsec;
    p_FTCalLog->q_GpsPeakCodePhMsec    = p_Gps->q_GpsPeakCodePhMsec;
    p_FTCalLog->q_GpsPeakFCount        = p_Gps->q_GpsPeakFCount;
    p_FTCalLog->f_GpsPeakCNodBHz       = p_Gps->f_GpsPeakCNodBHz;
    p_FTCalLog->q_GpsPeakLatency       = p_Gps->q_GpsPeakLatency;
    p_FTCalLog->f_GpsSpeedMperSec      = p_Gps->f_GpsSpeedMperSec;
    p_FTCalLog->q_GpsParams            = p_Gps->q_GpsParams;

    log_commit(p_FTCalLog );
    return TRUE;
  }
  else 
  {
    return FALSE;
  }
}

