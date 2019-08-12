#ifndef TM_IS801_H
#define TM_IS801_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  IS801 decode module

GENERAL DESCRIPTION
  This module handles IS801 specific routines

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/tm_is801.h#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/07   jw      Added prototype is801_get_rf_delay.
12/14/06   cl      Added req_mask and resp_mask in rev link pddm structure
07/27/06   cl      Initial version

============================================================================*/

#include "qw.h"
#include "tm_prtl_iface.h"
#include "tm_paii.h"
#include "custcgps.h"

#if 0 // comment out as OSS has been removed 
#ifdef FEATURE_CGPS_OSS_QDSP6
#include "is801_b_asn1_qdsp6.h"
#elif defined (T_WINNT)
#error code not present
#else
#include "is801_b_asn1.h"
#endif
#endif // #if 0

/* ------------------------------------------------------------------------
** Constant Definition Section
** ------------------------------------------------------------------------*/
#define MIN_IS801_PDDM_SIZE 58
#define MAX_IS801_PDDM_SIZE TM_MAX_BUFFSIZE

#define IS801_VERSION_UNKNOWN 0xff
#define IS801_VERSION_801_1   0x00
#define IS801_VERSION_801_A   0x01
#define IS801_VERSION_801_B   0x02

#define PD_MSG_TYPE 0x00       /* Position determination data message type */
#define PRDM_MSG_TYPE 0xc0     /* Proprietary Data Message Type */

/* Maximum num of req/resp per PDDM */
#define MAX_REQ_PER_PDDM   5
#define MAX_RESP_PER_PDDM  5

/* IS801 reverse-link request bit mask type */
typedef uint32                        is801_rev_link_req_mask_type;
#define IS801_REV_LINK_REQ_MASK_NONE  0x0000
#define IS801_REV_LINK_REQ_MASK_BSC   0x0001  /* BS Capabilities */
#define IS801_REV_LINK_REQ_MASK_AA    0x0002  /* Acquisition Assistance */
#define IS801_REV_LINK_REQ_MASK_LA    0x0004  /* Location Assistance */
#define IS801_REV_LINK_REQ_MASK_SA    0x0008  /* Sensitivity Assistance */
#define IS801_REV_LINK_REQ_MASK_BSA   0x0010  /* Base Station Almanac */
#define IS801_REV_LINK_REQ_MASK_ALM   0x0020  /* Almanac */
#define IS801_REV_LINK_REQ_MASK_EPH   0x0040  /* Ephemeris */
#define IS801_REV_LINK_REQ_MASK_NMB   0x0080  /* Navigation Message Bits */
#define IS801_REV_LINK_REQ_MASK_LR    0x0100  /* Location Response */
#define IS801_REV_LINK_REQ_MASK_AC    0x0200  /* GPS Almanac Correction */
#define IS801_REV_LINK_REQ_MASK_SHI   0x0400  /* GPS Satellite Health Information */
#define IS801_REV_LINK_REQ_NUM        11

/* IS801 reverse-link response bit mask type */
typedef uint32                         is801_rev_link_resp_mask_type;
#define IS801_REV_LINK_RESP_MASK_NONE  0x0000
#define IS801_REV_LINK_RESP_MASK_REJ   0x0001 /* Reject */
#define IS801_REV_LINK_RESP_MASK_MSI   0x0002 /* MS Information */
#define IS801_REV_LINK_RESP_MASK_AMWF  0x0004 /* Autonomous Measurement Weighting Factors */ 
#define IS801_REV_LINK_RESP_MASK_PRM   0x0008 /* Pseudorange Measurement */
#define IS801_REV_LINK_RESP_MASK_PPM   0x0010 /* Pilot Phase Measurement */
#define IS801_REV_LINK_RESP_MASK_LR    0x0020 /* Location Response */
#define IS801_REV_LINK_RESP_MASK_TOM   0x0040 /* Time Offset Measurement */
#define IS801_REV_LINK_RESP_MASK_CACK  0x0080 /* Cancellation Ack */
#define IS801_REV_LINK_RESP_MASK_PRDM_BSP 0x0100
#define IS801_REV_LINK_RESP_NUM            9

/* IS801 fwd-link request bit mask type */
typedef uint32                        is801_fwd_link_req_mask_type;
#define IS801_FWD_LINK_REQ_MASK_NONE  0x0000  /* NONE */
#define IS801_FWD_LINK_REQ_MASK_MSI   0x0001  /* MS Information */
#define IS801_FWD_LINK_REQ_MASK_AMWF  0x0002  /* Autonomous Measurement Weighting Factors */ 
#define IS801_FWD_LINK_REQ_MASK_PRM   0x0004  /* Pseudorange Measurement */
#define IS801_FWD_LINK_REQ_MASK_PPM   0x0008  /* Pilot Phase Measurement */
#define IS801_FWD_LINK_REQ_MASK_LR    0x0010  /* Location Response */
#define IS801_FWD_LINK_REQ_MASK_TOM   0x0020  /* Time Offset Measurement */
#define IS801_FWD_LINK_REQ_MASK_CAN   0x0040  /* Cancellation */


/* IS801 fwd-link response bit mask type */
typedef uint32                         is801_fwd_link_resp_mask_type;
#define IS801_FWD_LINK_RESP_MASK_NONE  0x0000  /* NONE */
#define IS801_FWD_LINK_RESP_MASK_REJ   0x0001  /* Reject */
#define IS801_FWD_LINK_RESP_MASK_BSC   0x0002  /* BS Capabilities */
#define IS801_FWD_LINK_RESP_MASK_AA    0x0004  /* Acquisition Assistance */
#define IS801_FWD_LINK_RESP_MASK_LASC  0x0008  /* Location Assistance Spherical Coordinates */
#define IS801_FWD_LINK_RESP_MASK_LACC  0x0010  /* Location Assistance Cartesian Coordinates */
#define IS801_FWD_LINK_RESP_MASK_SA    0x0020  /* Sensitivity Assistance */
#define IS801_FWD_LINK_RESP_MASK_BSA   0x0040  /* Base Station Almanac */
#define IS801_FWD_LINK_RESP_MASK_ALM   0x0080  /* Almanac */
#define IS801_FWD_LINK_RESP_MASK_EPH   0x0100  /* Ephemeris */
#define IS801_FWD_LINK_RESP_MASK_NMB   0x0200  /* Navigation Message Bits */
#define IS801_FWD_LINK_RESP_MASK_LR    0x0400  /* Location Response */
#define IS801_FWD_LINK_RESP_MASK_AC    0x0800  /* GPS Almanac Correction */
#define IS801_FWD_LINK_RESP_MASK_SHI   0x1000  /* GPS Satellite Health Information */

typedef struct
{
  byte sess_start;                           /* Start-of-session indicator */
  byte sess_end;                               /* End-of-session indicator */
  byte sess_source;                            /* Session-source indicator */
  byte sess_tag;                                            /* Session tag */
  byte pd_msg_type;            /* Position determination data message type */
  byte num_requests;        /* Number of request element parameter records */
  byte num_responses;      /* Number of response element parameter records */
} is801_pddm_hdr_type;


/* Reverse Link Req/Resp Sess Info */
typedef struct
{
  byte sess_start;                           /* Start-of-session indicator */
  byte sess_end;                               /* End-of-session indicator */
  byte sess_source;                            /* Session-source indicator */
  byte sess_tag;                                            /* Session tag */
  byte pd_msg_type;            /* Position determination data message type */  
} is801_pddm_sess_info_s_type;



/****************************************************************************
                 
          Forward Link Request Data Structure Definition Section
 
****************************************************************************/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST PSEUDORANGE MEASUREMENT                                         */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0100')  */
typedef struct
{
  byte                pref_resp_qual;                   /* Preferred response quality */
  byte                num_fixes;                        /* Number of fixes */
  byte                t_betw_fixes;                     /* Time between fixes */
  byte                offset_req;                       /* Offset requested */
} is801_fwd_link_req_prm_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST PILOT PHASE MEASUREMENT                                         */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0101')  */

typedef struct
{
  byte                pref_resp_qual;                   /* Preferred response quality */
  byte                num_fixes;                        /* Number of fixes */
  byte                t_betw_fixes;                     /* Time between fixes */
  byte                offset_req;                       /* Offset requested */
  byte                desi_pil_ph_res;                  /* Desired pilot phase resolution */
} is801_fwd_link_req_ppm_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST LOCATION RESPONSE                                               */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0001')  */

typedef struct
{
  byte                pref_resp_qual;                   /* Preferred response quality */
  byte                num_fixes;                        /* Number of fixes */
  byte                t_betw_fixes;                     /* Time between fixes */
  byte                height_req;                       /* Height information requested */
  byte                clk_cor_gps_req;                  /* Clock correction for GPS time requested */
  byte                velocity_req;                     /* Velocity information requested */
} is801_fwd_link_req_lr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST TIME OFFSET MEASUREMENT                                         */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0110')  */

typedef struct
{
  byte                use_time;                          /* Use action time indicator */
  byte                action_time;                       /* Action time */
} is801_fwd_link_req_tom_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST CANCELLATION                                                    */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0111')  */

typedef struct
{
  byte                cancel_type;                       /* Cancellation type */
} is801_fwd_link_req_cancel_type;



/* Union for fwd-link request types */
typedef union
{
  is801_fwd_link_req_prm_type     prm_req;
  is801_fwd_link_req_ppm_type     ppm_req;
  is801_fwd_link_req_lr_type      lr_req;
  is801_fwd_link_req_tom_type     tom_req;
  is801_fwd_link_req_cancel_type  cancel_req;
} is801_fwd_link_req_msg_payload_u_type;


/****************************************************************************
                 
        Forward Link Response Data Structure Definition Section
 
****************************************************************************/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REJECT                  (See IS-801 Section 3.2.4.2 RESP_TYPE = '0000') */
typedef struct
{
  byte                 rej_req_type;              /* Reject request type */
  byte                 rej_reason;                /* Reject reason */
} is801_rej_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE LOCATION RESPONSE                                               */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='0001') */

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

  struct
  {
    uint8               u_SvInfoValid;      /* TRUE if all info in this struct valid, otherwise FALSE */

    uint8               u_GpsNumSvMeas;     /* Number of GPS SVs with valid measurement (detected by searcher) */
    uint8               u_GpsNumSvPosFix;   /* Number of GPS SVs used in Position Fix */
    uint8               u_GpsNumSvVelFix;   /* Number of GPS SVs used in Velocity Fix */
    uint32              q_GpsSvMaskUsed;    /* Bitmask for GPS SVs used for calculating position fix */ 
    uint32              q_GpsSvMaskUsable;  /* Bitmask for GPS SVs usable for calculating position fix */
    pdsm_GnssSvInfoType z_GpsSvInfo[PDSM_N_ACTIVE_GPS_CHAN]; /* SV-specific data for GPS supplemental SV information */
  } z_NavSvInfo;
  
} is801_lr_resp_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS ACQUISITION ASSISTANCE                                      */
/*                           (see IS-801 Page 4-27 RESP_TYPE='0100') */
  
typedef struct
{
  byte    num_sv;                            /* Number of GPS satellites */
  byte    time_of_app;       /* CDMA system time ref for the GPS AA info */
  byte    dopp_incl;                /* Doppler (0th order) term included */
  byte    add_dopp_incl;             /* Additional doppler info included */
  byte    code_ph_par_incl;                  /* Code phase info included */
  uint16  reference_pn;                                  /* Reference PN */
  byte    az_el_incl;            /* Azimuth and elevation angle included */
} is801_aa_fix_type;
  
typedef struct
{
  byte    sv_prn_num;                            /* Satellite PRN number */      
  sint15  doppler0;                                 /* 0th order doppler */
  int1    doppler1;                                 /* 1st order doppler */
  uint16  doppler_win_0_5_hz;                   /* Doppler search window */
  uint16  sv_code_ph;                                  /* GPS code phase */
  byte    sv_code_ph_int;              /* Integer number of code periods 
                                                 since last GPS bit boundary */
  byte    gps_bit_num; /* 2 least significant bits of bit number (within 
                                  the GPS frame) currently being transmitted */
  uint16  sv_code_ph_win;                     /* Total code phase window */
  byte    azimuth;                       /* Azimuth of the GPS satellite */
  byte    elevation;             /* Elevation angle of the GPS satellite */
} is801_aa_var_type;
  
typedef struct
{
  qword                timestamp; /* CDMA time that the data was received */
  is801_aa_fix_type    fix;    
  is801_aa_var_type    var[MAX_NUM_SATS];         /* Satellite AA records */
} is801_aa_resp_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS SENSITIVITY ASSISTANCE                                      */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='0101') */
  
typedef struct
{
  uint16  ref_bit_num;                           /* Reference bit number */
  byte    dr_size;                                   /* Data record size */
  byte    nav_msg_bits[MAX_NAV_BITS_IN_BYTES];/* Navigation message bits */
  byte    num_sv_dr;     /* Number of GPS satellites in this data record */
  byte    sv_prn_num[MAX_NUM_SATS_IN_DATA_RECORD]; /* Satellite PRN nums */
} is801_sa_dr_type;
  
typedef struct
{
  qword           timestamp; /* CDMA time that the data was received */    
  byte            num_data_records;            /* Number of data records */
  is801_sa_dr_type data[MAX_DATA_RECORDS];             /* SA data records */
} is801_sa_resp_type;
   

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS ALMANAC RESPONSE                                            */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='1000') */
/* Ordering of structure elements is different than IS-801 message format  */
/* to accomodate port of MMGPS PE                                          */
typedef struct
{
  uint8   u_SatIndex;
  uint8   u_TotalSats;      /* num_sv_p - Number of data records in part */
  uint8   u_Part_Num;                             /* Number of this part */
  uint8   u_Total_Parts;                        /* Total number of parts */
  uint8   u_WeekNum;                                  /* GPS week number */
  uint8   u_Toa;                                      /* Time Of Almanac */ 
  uint8   u_Week;                                     /* GPS week number */
} is801_alm_part_fix_type;


typedef struct
{
  uint8   u_Sv_Prn_Num;                          /* Satellite PRN number */
  uint8   u_Health;
  uint16  w_DeltaI;                         /* Correction to inclination */
  uint32  q_MZero;                     /* Mean anomaly at reference time */
  uint16  w_E;                                           /* Eccentricity */
  uint32  q_SqrtA;                 /* Square root of the semi-major axis */
  uint32  q_OmegaZero;     /* Longitude of ascending node of orbit plane */
  uint32  q_Omega;                                /* Argument of perigee */
  uint16  w_OmegaDot;                         /* Rate of right ascension */
  uint16  w_Af1;              /* Apparant satellite clock correction af1 */
  uint16  w_Af0;              /* Apparant satellite clock correction af0 */
  uint8   u_Toa;                                      /* Time Of Almanac */ 
  uint8   u_WeekNum;                                  /* GPS week number */
} is801_alm_part_var_type;       

typedef struct
{
  qword                    timestamp;  /* CDMA time the data was received */    
  is801_alm_part_fix_type  fix;
  is801_alm_part_var_type  var[MAX_NUM_ALM_SATS];
} is801_alm_resp_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS EPHEMERIS RESPONSE                                          */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='1001') */

typedef struct
{
  uint8 u_SatIndex;
  uint8 u_TotalSats;                   /* Number of data records in part */
  uint8 u_Part_Num;                               /* Number of this part */
  uint8 u_Total_Parts;                          /* Total number of parts */
  uint8 u_AB_Par_Incl;             /* Alpha and beta parameters included */
  uint8 u_Alpha_0;           /* Ionospheric correction parameter alpha 0 */
  uint8 u_Alpha_1;           /* Ionospheric correction parameter alpha 1 */
  uint8 u_Alpha_2;           /* Ionospheric correction parameter alpha 2 */
  uint8 u_Alpha_3;           /* Ionospheric correction parameter alpha 3 */
  uint8 u_Beta_0;            /* Ionospheric correction parameter beta  0 */
  uint8 u_Beta_1;            /* Ionospheric correction parameter beta  1 */
  uint8 u_Beta_2;            /* Ionospheric correction parameter beta  2 */
  uint8 u_Beta_3;            /* Ionospheric correction parameter beta  3 */
  double  d_UpdateTime; 
} is801_eph_part_fix_type;

typedef struct
{
  uint8 u_Sv_Prn_Num;                            /* Satellite PRN number */
  uint8 u_Iode;                                         /* Issue of data */
  int16 x_C_Rs;        /* Amplitude of the sine harmonic correction term */
  int16 x_Delta_N;                             /* Mean Motion Difference */
  int32 l_M0;                     /* Mean anom,aly at the reference time */
  int16 x_C_Uc;      /* Amplitude of the cosine harmonic correction term */
  int32 l_Eccentricity;                                  /* Eccentricity */
  int16 x_C_Us;        /* Amplitude of the sine harmonic correction term */
  int32 l_A_Sqrt;                  /* Square root of the semi-major axis */
  int16 x_Toe;                               /* Ephemeris reference time */
  int16 x_C_Ic;      /* Amplitude of the cosine harmonic correction term */
  int32 l_Omega_0;         /* Longitude of ascending node of orbit plane */
  int16 x_C_Is;        /* Amplitude of the sine harmonic correction term */
  int32 l_Angle_Inclination;  /* Inclination angle at the reference time */
  int16 x_C_Rc;      /* Amplitude of the cosine harmonic correction term */
  int32 l_Omega;                                  /* Argument of perigee */
  int32 l_Omegadot;                           /* Rate of right ascension */
  int16 x_Idot;                             /* Rate of inclination angle */
  int16 x_Toc;                              /* Clock data reference time */
  uint8 b_Af2;                /* Apparant satellite clock correction af2 */
  int16 b_Af1;                /* Apparant satellite clock correction af1 */
  int32 l_Af0;                /* Apparant satellite clock correction af0 */
} is801_eph_part_var_type;       

typedef struct
{
  qword                   timestamp;  /* CDMA time the data was received */    
  is801_eph_part_fix_type  fix;
  is801_eph_part_var_type  var[MAX_NUM_EPH_SATS];
} is801_eph_resp_type;


/* Union for fwd link response types */
typedef union
{
  is801_rej_resp_type    rej_resp;
  is801_aa_resp_type     aa_resp;
  is801_lr_resp_type     lr_resp;
  is801_sa_resp_type     sa_resp;
  is801_alm_resp_type    alm_resp;
  is801_eph_resp_type    eph_resp;
} is801_fwd_link_resp_msg_payload_u_type;

/****************************************************************************************************************

                                    Reverse Link Request/Response Section
                                  
*****************************************************************************************************************/
/* PROVIDE MS INFORMATION    (see IS-801 Section 3.2.4.2 RESP_TYPE='0010') */                                       
typedef struct
{
  byte    ms_ls_rev;                /* Location standard revision number */
  byte    ms_mode;                             /* Digital mode indicator */
  byte    pilot_ph_cap;                        /* Pilot phase capability */
  word    gps_acq_cap;                     /* GPS acquisition capability */
  word    loc_calc_cap;               /* Location calculation capability */
} is801_msi_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE PSEUDORANGE MEASUREMENT                                         */
/*                           (see IS-801 Section 3.2.4.2 RESP_TYPE='0100') */
typedef struct
{
  byte            part_num;                               /* Part number */
  byte            total_parts;                        /* Number of parts */
  byte            num_ps_ranges_p;/* Number of Pseudoranges in this part */
  uint32          time_ref;                /* Time validity of this part */
  byte            time_ref_src;                 /* Time reference source */
  byte            offset_incl;                        /* Offset included */
  uint16          ref_pn;                  /* Time reference PN sequence */
  sint15          mob_sys_t_offset;             /* MS system time offset */
} is801_prm_part_fix_type;
  
typedef struct
{
  byte    sv_prn_num;                            /* Satellite PRN number */
  byte    sv_cno;                                      /* Satellite C/No */
  sint15  ps_doppler;                                   /* Pseudodoppler */
  uint16  sv_code_ph_wh;           /* Satellite code phase - whole chips */
  uint16  sv_code_ph_fr;      /* Satellite code phase - fractional chips */
  byte    mul_path_ind;                           /* Multipath indicator */
  byte    ps_range_rms_er;                      /* Pseudorange RMS error */
} is801_prm_part_var_type;
  
typedef struct
{
  is801_prm_part_fix_type   fix;
  is801_prm_part_var_type   var[MAX_NUM_SAT_MEASUREMENTS];     /* Sat recs */
} is801_prm_resp_type;
  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE PILOT PHASE MEASUREMENT                                         */
/*                           (see IS-801 Section 3.2.4.2 RESP_TYPE='0101') */

typedef struct
{
  uint32          time_ref_ms;             /* Time validity of this part */
  byte            offset_incl;                        /* Offset included */
  sint15          mob_sys_t_offset;             /* MS system time offset */
  uint16          ref_pn;           /* Time reference PN sequence offset */
  byte            ref_pilot_strength;                  /* Pilot strength */
  byte            band_class;                              /* Band class */
  uint16          cdma_freq;                      /* Requency assignment */
  uint16          base_id;                /* Base station identification */
  uint16          sid;                          /* System identification */
  uint16          nid;                         /* Network identification */
  byte            total_rx_pwr;                  /* Total received power */
  byte            part_num;                               /* Part number */
  byte            total_parts;                        /* Number of parts */
  byte            num_pilots_p;         /* Number of pilots in this part */
  byte            add_pilots_incl;         /* Additional pilots included */
  byte            add_band_class;               /* Additional band class */
  word            add_cdma_freq;      /* Additional frequency assignment */
  byte            add_total_rx_pwr;/* Tot received power on the add freq */
  byte            num_add_pilots_p;/* Num additional pilots in this part */
} is801_ppm_part_fix_type;

typedef struct
{
  uint32  pilot_pn_phase;                        /* Pilot measured phase */
  byte    pilot_strength;                              /* Pilot strength */
  byte    rms_err_phase;               /* RMS error in phase measurement */
} is801_ppm_part_var_type;

typedef struct
{
  is801_ppm_part_fix_type  fix;
  is801_ppm_part_var_type  var[MAX_NUM_PILOT_MEASUREMENTS]; /* Pilot recs */
} is801_ppm_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE TIME OFFSET MEASUREMENT                                         */
/*                         (See IS-801 Section 3.2.4.2 RESP_TYPE = '0110') */
typedef struct
{
  uint32                time_ref_ms;      /* Time validity of this message */
  uint16                ref_pn;           /* Time reference PN offset */
  uint16                mob_sys_t_offset; /* MS system time offset */
} is801_tom_resp_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE CANCELLATION ACKNOWLEDGEMENT                                    */
/*                         (See IS-801 Section 3.2.4.2 RESP_TYPE = '0111') */
typedef struct
{
  byte                cancel_type_ack;              /* Cancellation type */
  byte                no_out_req;      /* No outstanding request element */
} is801_cack_resp_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*         PROPRIETARY DATA MESSAGE TYPES (pd_msg_type==0xc0)              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*************************************************************************
** NOTE: The request and response header format for Proprietary data msgs
** are the same as the regular Position Determination Data Messages.
**************************************************************************/

/* BS System Parameter Info MSG (RESP_TYPE='1110') */

typedef struct
{
  uint16 time_ref_cdma;            /* CDMA time when sys param is received */
  byte   still_active;             /* Base Station in Active Set indicator */
  byte   band_class;               /* Base station class */
  uint16 cdma_freq;                /* CDMA frequency */
  uint16 pilot_pn;                 /* Pilot PN sequence offset index */
  uint16 sid;                      /* System identification */
  uint16 nid;                      /* Network identification */
  uint16 base_id;                  /* Base station identification */
  uint32 base_lat;                 /* Base station latitude */
  uint32 base_long;                /* Base station longitude */
} is801_prdm_bsp_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MS REV DATA MSG (RESP_TYPE='0001')                                      */

typedef struct
{
    uint16 chip_id;                  /* Chip ID */
    uint32 sw_rev_id;                /* Software Revision ID */
    uint32 dev_manufacturer;         /* Device manufacturer */
    uint32 dev_rev_id;               /* Device Revision ID */
} is801_prdm_ms_rev_resp_type;

/* is801 reverse link resp type */
typedef union
{
  is801_msi_resp_type     msi_resp;
  is801_prm_resp_type     prm_resp;  
  is801_ppm_resp_type     ppm_resp;  
  is801_tom_resp_type     tom_resp;
  is801_lr_resp_type      lr_resp;    
  is801_cack_resp_type    cack_resp;
  is801_rej_resp_type     rej_resp; 
  is801_prdm_bsp_resp_type     prdm_bsp_resp; 
} is801_rev_link_resp_msg_payload_u_type;

/* is801 reverse link resp type */
typedef struct
{ 
  is801_rev_link_resp_mask_type           response_type;
  boolean                                 unsolicited;  
  is801_rev_link_resp_msg_payload_u_type  resp_msg_payload;  
} is801_rev_link_resp_s_type;


/* is801 forward link request type */
typedef struct
{ 
  is801_fwd_link_req_mask_type           request_type;
  is801_fwd_link_req_msg_payload_u_type  req_msg_payload;        /* Place holder for processed IS801 Request (unpacked) */
} is801_fwd_link_req_s_type;

/* is801 forward link resp type */
typedef struct
{ 
  is801_fwd_link_resp_mask_type          resp_type;
  boolean                                unsolicited;  
  is801_fwd_link_resp_msg_payload_u_type resp_msg_payload;        /* Place holder for processed IS801 Response (unpacked) */
} is801_fwd_link_resp_s_type;

typedef struct
{
  is801_pddm_hdr_type                     hdr;
  is801_fwd_link_req_mask_type            req_mask;
  is801_fwd_link_resp_mask_type           resp_mask;  
  is801_fwd_link_req_s_type               req_rd[MAX_REQ_PER_PDDM];
  is801_fwd_link_resp_s_type              resp_rd[MAX_RESP_PER_PDDM];
} is801_fwd_link_pddm_info_s_type;

typedef struct
{
  byte                                     msg_size;
  is801_rev_link_req_mask_type             req_mask;
  is801_rev_link_resp_mask_type            resp_mask;    
  byte                                     msg_payload[MAX_IS801_PDDM_SIZE];
} is801_rev_link_pddm_info_s_type;

typedef struct
{
  uint8                         num_pddm; 
} is801_fwd_pddm_info_s_type;

typedef struct
{
  boolean*             p_FTCalEnabled;
  cgps_FtcalPpmStruct* p_Ppm;
  cgps_FtcalTTStruct*  p_TT;
} is801_FtcalDataStruct;

/*===========================================================================

FUNCTION: prtl_is801_msg_cb_f_type

DESCRIPTION:
  This function is called by IS801 module to protocol sub-module to inform
  PDDM data is available.
  
PARAMETERS:
  
===========================================================================*/
typedef boolean (prtl_is801_msg_cb_f_type) 
(
  is801_fwd_pddm_info_s_type    pddm_info
);


/*===========================================================================

FUNCTION is801_init

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801_init(void);


/*===========================================================================

FUNCTION is801_prtl_reg

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_prtl_reg
(
                              tm_prtl_type              prtl_type,
                              prtl_is801_msg_cb_f_type  *msg_cb_fp
                            );


/*===========================================================================

FUNCTION is801_decode_fwd_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_decode_fwd_link_pddm
(
                                        tm_prtl_type  prtl_type,
                                        byte          *data,
                                        uint16        length,
                                        byte          *offset
                                       );

/*===========================================================================

FUNCTION is801_new_rev_link_req

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_new_rev_link_req(
                                    tm_prtl_type                  prtl_type,                             
                                    is801_rev_link_req_mask_type  req_type
                                  );

/*===========================================================================

FUNCTION is801_new_rev_link_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_new_rev_link_resp(
                                     tm_prtl_type                    prtl_type,
                                     is801_rev_link_resp_s_type      *resp_msg
                                   );

/*===========================================================================

FUNCTION is801_create_rev_link_pddm

DESCRIPTION
 When protocol sub-modules creates rev-link PDDM, it also needs to tell IS801 
 module the maximum size of PDDM, which is limited by each protocol sub-modules'
 transport layer. 

 For example, 1x cp uses DBM as its transport layer, and the maximum dbm size 
 can differ from network to network.
 
DEPENDENCIES

RETURN VALUE
  Number of PDDM created.
  -1 indicates there is error when creating PDDM.

  Client should then call is801_GetRevLinkPDDM to get packed PDDM data to send
  over to PDE.

SIDE EFFECTS

===========================================================================*/
extern int is801_create_rev_link_pddm(
                                    tm_prtl_type                 prtl_type,
                                    is801_pddm_sess_info_s_type  msg_param,
                                    uint8                        max_pddm_size
                                  );


/*===========================================================================

FUNCTION is801_get_fwd_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_get_fwd_link_pddm
(
                                     tm_prtl_type                     prtl_type,
                                     is801_fwd_link_pddm_info_s_type  *fwd_link_pddm_buf
                                   );

/*===========================================================================

FUNCTION is801_get_rev_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean is801_get_rev_link_pddm
(
                                     tm_prtl_type                     prtl_type,
                                     is801_rev_link_pddm_info_s_type  *pddm_info_ptr
                                   );

/*===========================================================================

FUNCTION is801_reset_data

DESCRIPTION
  This function needs to be called when client finishes a session
  for any reason.

  It ensures that all the data structures are re-initiailzed for future IS801 sessions.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801_reset_data(
                             tm_prtl_type prtl_type
                           );


/*===========================================================================

FUNCTION is801_get_rf_delay

DESCRIPTION    This function updates the band class and associated RF delay.

DEPENDENCIES   None.

RETURN VALUE   RF Delay for the band class in GPS Cx1024

SIDE EFFECTS   None.

===========================================================================*/

extern int32 is801_get_rf_delay
(
  uint8 band_class
);

/*===========================================================================

FUNCTION tm_is801_ftcal_enabled

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

extern void tm_is801_ftcal_enabled
(
  boolean b_Enable
);

/*===========================================================================

FUNCTION tm_is801_ftcal_tt_data

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

extern void tm_is801_ftcal_tt_data
(
  cgps_FtcalTTStruct*
);

/*===========================================================================

FUNCTION tm_is801_ftcal_ppm_data

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

extern void tm_is801_ftcal_ppm_data
(
  cgps_FtcalPpmStruct*
);


/*===========================================================================

FUNCTION tm_is801_get_ftcal_data

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

void tm_is801_get_ftcal_data ( is801_FtcalDataStruct * p_FtcalData );

#ifdef __cplusplus
}
#endif

#endif /* TM_IS801_H */

