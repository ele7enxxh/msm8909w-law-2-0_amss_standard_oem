#ifndef TDSRRCLSMIF_H
#define TDSRRCLSMIF_H

/*========================================================================================

   RRC - LSM INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by RRC and LSM

  Copyright (c) 2010-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tdsrrclsmif.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
03/10/11   ysh    Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53
02/10/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11    ad      Added changes to refeaturize the API files
01/28/11    rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11    ad      Added changes for RRC SW decoupling
10/30/10    rl      Objective systems migration changes check in 
                    on code base VU_MODEM_WCDMA_RRC.01.80.00
08/24/10    rm      Added GPS reference time uncertainity field to 
                    rrc_gps_measured_results_struct_type

========================================================================================*/



#include "tdsrrcmmif.h"

#define TDSMAX_NO_OF_SATELLITES 16
#define TDSGLOBAL_HEALTH_ARRAY_SIZE 46





/* Type of parameters */
typedef enum
{
  /* Measurement parameters from SIB 11/12 */
  TDSRRC_SIB_PARMS,
  /* Measurement parameters from Measurement Control Message */
  TDSRRC_MEAS_CTRL_PARMS,
   /* Measurement parameters from Assistance Data Delivery*/
  TDSRRC_ASSISTANCE_DATA_PARMS,
  /* Measurement parameters from RRC on state transition from DCH */
  TDSRRC_STATE_CHANGE_PARMS
} tdsrrc_meas_params_enum_type;


/* Command Action */
typedef enum
{
  TDSRRC_MEAS_SETUP = 1,  /* Start A-GPS Measurements  */
  TDSRRC_MEAS_MODIFY,     /* Modify an existing measurement  */
  TDSRRC_MEAS_DELETE,     /* Delete meas */
  TDSRRC_MEAS_DELETE_ALL, /* Deletes all the existing measurement ids   */
  TDSRRC_MEAS_SAVE,       /* Stop & Save the measurement info */
  TDSRRC_MEAS_RESUME,      /* Resume measurement */
  TDSRRC_CONTINUE_CHANGE_PERIODIC_INTERVAL,
  TDSRRC_CHANGE_PERIODIC_INTERVAL_TO_PCH,   /* Change Periodic Reporting Interval to 64 secs */
  TDSRRC_CHANGE_PERIODIC_INTERVAL_FROM_PCH /* change back from 64s to the earlier stored value */
} tdsrrc_meas_cmd_enum_type;

/* Positioning Method Type */
typedef enum
{
  TDSRRC_UE_ASSISTED = 0,
  TDSRRC_UE_BASED,
  TDSRRC_UE_BASED_PREFERED,
  TDSRRC_UE_ASSISTED_PREFERED
} tdsrrc_positioning_method_type_enum_type;

/* Positioning Method */
typedef enum
{
  TDSRRC_OTDOA = 0,
  TDSRRC_GPS,
  TDSRRC_OTDOAorGPS,
  TDSRRC_CELLID
} tdsrrc_positioning_method_enum_type;

/* Navigation Model of Satellite */
typedef enum
{
  TDSRRC_NEW_SATELLITE__NEW_NAVIFGATION_MODEL = 0,
  TDSRRC_EXISTING_SATELLITE_SAME_NAVIGATION_MODEL,
  TDSRRC_EXISTING_SATELLITE_NEW_NAVIGATION_MODEL
} tdsrrc_satellite_status_enum_type;


/* Doppler Uncertainty in Hz */
typedef enum
{
   TDSRRC_HZ12_5 = 0, /* 12.5 */
   TDSRRC_HZ25, /* 25 */
   TDSRRC_HZ50, /* 50 */
   TDSRRC_HZ100, /* 100 */
   TDSRRC_HZ200 /* 200 */
} tdsrrc_doppler_uncertainity_enum_type;

/* Error Cause */
typedef enum
{
  /* LSM sets it if it finds invalid configuration for
     A-GPS Meas with actual Meas Params */
  TDSCONFIGURATION_INVALID
} tdsrrc_error_cause_enum_type;



/* Positioning Estimate is based on this type */
typedef enum
{
  TDSUTRAN_GPS_REFERENCE_TIME = 1,
  TDSGPS_REFERENCE_TIME_ONLY,
  TDSCELL_TIMING_ONLY
} tdsrrc_positioning_estimate_enum_type;


/* SFN-TOW Uncertainty. LessThan10 means the relation is accurate to at least 10 ms.*/
typedef enum
{
  TDSLESS_THAN_10,
  TDSMORE_THAN_10
} tdsrrc_sfn_tow_uncertainity_enum_type;

/* SFN-TOW Uncertainty. LessThan10 means the relation is accurate to at least 10 ms.*/
typedef enum
{
  TDSNORTH,
  TDSSOUTH
} tdsrrc_latitude_sign_enum_type;

/* Altitude Direction.*/
typedef enum
{
  TDSHEIGHT,
  TDSDEPTH
} tdsrrc_altitude_direction_enum_type;

/* Type of Reporting */
typedef enum
{
  TDSRRC_EVENT_TRIGGER_RPT = 1,
  TDSRRC_PERIODIC_RPT,
  TDSRRC_NO_RPT /* Additional Measurement */
} tdsrrc_report_crit_enum_type;



/* Report Transfer Mode */
typedef enum
{
  TDSMODE_ACKNOWLEDGED,
  TDSMODE_UNACKNOWLEDGED
} tdsrrc_report_transfer_mode_enum_type;


/* Envrionment Ch */
typedef enum
{
  TDSPOSSIBLE_HEAVY_MULTIPATH_LOS,
  TDSLIGHT_MULTIPATH_LOS
} tdsrrc_environment_characterisation_enum_type;

/* Positioning Error Codes used by RRC and LSM */
typedef enum
{
  TDSRRC_NOT_ENOUGH_OTDOA_CELLS = 0,
  TDSRRC_NOT_ENOUGH_GPS_SATELLITES,
  TDSRRC_ASSISTANCE_DATA_MISSING,
  TDSRRC_METHOD_NOT_SUPPORTED,
  TDSRRC_UNDEFINED_ERROR,
  TDSRRC_REQUEST_DENIED_BY_USER,
  TDSRRC_NOT_PROCESSED_AND_TIMEOUT,
  TDSRRC_REFERENCE_CELL_NOT_SERVING_CELL
} tdsrrc_positioning_error_cause;


/* Multipath Type */
typedef enum
{
  TDSNOT_MEASURED,  /*   Not measured */
  TDSLOW,           /* Multipath error < 5m */
  TDSMEDIUM,        /* 5m < Multipath error < 43m */
  TDSHIGH           /* Multipath error > 43m */
} tdsrrc_mulitpath_enum_type;

typedef enum
{
  TDSELLIPSOID_POINT = 1,
  TDSELLIPSOID_POINT_UNCERTAIN_CIRCLE,
  TDSELLIPSOID_POINT_UNCERTAIN_ELLIPSE,
  TDSELLIPSOID_POINT_ALTITUDE,
  TDSELLIPSOID_POINT_ALTITUDE_ELLIPSE
} tdsrrc_pos_estimate_result_enum_type;

/* State Transition Command */


typedef PACKED struct PACKED_POST
{
  /* Here Cmd Type can only 5 values i.e.
  TDSRRC_MEAS_DELETE,
  TDSRRC_MEAS_DELETE_ALL,
  TDSRRC_MEAS_SAVE,
  TDSRRC_MEAS_RESUME,
  TDSRRC_CONTINUE_CHANGE_PERIODIC_INTERVAL     */
  uint8 trans_cmd_type; /* Of tdsrrc_meas_cmd_enum_type */
  uint8 meas_id;
} tdsrrc_state_trans_struct_type;

/* Cell Timing Only */
typedef PACKED struct PACKED_POST
{
  /* SFN of current cell */
  uint32 sfn;
  /* Primary Scrambling Code of current cell */
  uint8 cpid;

} tdsrrc_cell_timing_struct_type;

/* GPS UMTS Reference Time */

typedef PACKED struct PACKED_POST
{
  /* UTRAN GPS timing of cell frames in steps of 1 chip */

  /* Actual Value for UTRAN GPS timing is (msb_timing * 4294967296) + lsb_timing */
  /* msb_timing (0..1023),lsb_timing (0..4294967295) */
  uint16 msb_timing;
  uint32 lsb_timing;


  boolean  gps_ref_cpid_incl;
  /* Reference cell for the GPS TOW-SFN relationship */
  uint8 cpid; /* 0..127 */

  /* SFN at which the UTRAN GPS timing of cell frames time stamps */
  uint32 sfn; /* 0..4095 */

} tdsrrc_gps_umts_reference_time_struct_type;

/* GPS Reference Location i.e. Ellipsoid point with altitude and uncertainty ellipsoid */
typedef PACKED struct PACKED_POST
{
  /* North or South */
  uint8 latitude_sign; /* Of tdsrrc_latitude_sign_enum_type */

  /* RRC QUERY pending for the following values, waiting for clarification */

  /* Latitude in Degrees*/
  uint32 latitude; /* 0...223-1     N 223 X /90   N+1 (0..8388607) */

  /* Longitude in Degrees*/
  int32 longitude; /* 0...223-1    N 224 X /360   N+1 (-8388608..8388607)*/

  /* Altitude Direction */
  uint8 altitude_direction; /* Height or Depth, tdsrrc_altitude_direction_enum_type */

  /* Altitude */
  uint16 altitude; /* 0..215-1      N<a<N+1 (0..32767) (0..32767) */

  /* Uncertainity Semi Major*/
  uint8 uncertainity_semi_major; /* (0..127) */

  /* Uncertainity Semi Minor*/
  uint8 uncertainity_semi_minor; /* (0..127) */

  /* Orientation Major Axis*/
  uint8 orientation_major_axis; /* IE value is (0..89). RRC multiplies by 2 */

  /* Uncertainity Altitude */
  uint32 uncertainity_altitude; /* (0..127) */

  /* Confidence*/
  uint32 confidence; /* In percentage (0..100) */

} tdsrrc_gps_reference_loc_struct_type;

/* Positioning Estimate */
typedef PACKED struct PACKED_POST
{
  uint8 positioning_estimate; /* tdsrrc_positioning_estimate_enum_type */
  PACKED union PACKED_POST
  {
    /* GPS Reference Time Only */
    uint32 gps_reference_time;

    /* Cell Timing Only */
    tdsrrc_cell_timing_struct_type cell_timing;

    /* UTRAN GPS Reference Time */
    tdsrrc_gps_umts_reference_time_struct_type utran_gps_ref_time;

  } u;

  uint8 pos_estimate_choice; /* tdsrrc_pos_estimate_result_enum_type */

  tdsrrc_gps_reference_loc_struct_type gps_ref_location;

} tdsrrc_positioning_estimate_struct_type;




/* GPS Measured Param List for each satellite */

typedef PACKED struct PACKED_POST
{
  /* Satellite Identity */
  uint8 sat_id; /* 0 to 63 */

  /* Estimate of the carrier-to-noise ratio of the received signal
     from the particular satellite used in the measurement.
     It is given in units of dB-Hz */
  uint8 c_no; /* 0 to 63 */

  /* Doppler */
  int32 doppler; /* -32768 to 32768 */

  /* Whole GPS chips */
  uint16 whole_gps_chips; /* 0 to 1022 */

  /* Fractional GPS Chips */
  uint16 fractional_gps_chips; /* RRC QUERY Conversion Reqd 0 to 1023 */

  /* Multipath Indication */
  uint8 multipath_ind; /* rrc_multipath_enum_type */

  /* LSM to give converted value */
  uint32  rms_error; /* 0 to 63 */

} tdsrrc_gps_measured_param_list_struct_type;


/* GPS UMTS Reference Time Results */

typedef PACKED struct PACKED_POST
{
  /* GPS UTRAN reference Time */
  tdsrrc_gps_umts_reference_time_struct_type gps_umts_reference_time;

} tdsrrc_gps_umts_ref_time_meas_res_struct_type;

/* GPS Measured Results */
typedef PACKED struct PACKED_POST
{
  /* Value can only be GPS reference tIme or GPS-UTRAN refernece Time */
  uint8 positioning_estimate; /* tdsrrc_positioning_estimate_enum_type */

  PACKED union PACKED_POST
  {
    /* GPS Reference Time Only */
    uint32 gps_reference_time;

    /* UTRAN GPS Reference Time */
    tdsrrc_gps_umts_ref_time_meas_res_struct_type gps_umts_meas_results;

  } u;

  /* No of satellites */
  uint8 no_of_satellites;

  /* GPS Reference Time uncertainity */
  boolean gps_reference_time_uncertainity_incl;
  uint8 gps_reference_time_uncertainity;

  /* GPS Measured Param List for each satellite */
  tdsrrc_gps_measured_param_list_struct_type gps_measured_params[TDSMAX_NO_OF_SATELLITES];

} tdsrrc_gps_measured_results_struct_type;

/* Satellite related data */
typedef PACKED struct PACKED_POST
{
  /* Satellite Identity */
  uint8 sat_id; /* 0 to 63 */

  /* Issue of Data Ephemeris */
  uint8 iode; /* 0 to 255 */

} tdsrrc_gps_satellite_related_data_struct_type;



/* Additional Nav Data */
typedef PACKED struct PACKED_POST
{
  /* GPS Week */
  uint16 gps_week; /* 0 to 1023 */

  /* GPS time of ephemeris in hours of the latest ephemeris set */
  uint8 gps_tow; /* 0 to 167 */

  /* Ephemeris age tolerance of the UE in hours */
  uint8 age_tolerance;  /* 0 to 10 */

  /* No of satellites */
  uint8 no_of_satellites;

  /* Satellite realted data */
  tdsrrc_gps_satellite_related_data_struct_type sat_data[TDSMAX_NO_OF_SATELLITES];

} tdsrrc_gps_nav_data_struct_type;

/* Indicates what type of additional data is required */

typedef PACKED struct PACKED_POST
{
  boolean almanac_request_incl;

  boolean utc_model_request_incl;

  boolean ionospheric_model_request_incl;

  boolean navigation_model_request_incl;

  boolean dgps_corrections_request_incl;

  boolean reference_location_request_incl;

  boolean reference_time_request_incl;

  boolean aquisition_assistance_request_incl;

  boolean real_time_integrity_request_incl;

  boolean nac_model_data_req_incl;
  tdsrrc_gps_nav_data_struct_type navModelAddDataRequest;

} tdsrrc_additional_assistance_data_struct_type;


/* Positioning Error */

typedef PACKED struct PACKED_POST
{
  /* Positioning Error Cause */
  uint8 error_cause; /* tdsrrc_positioning_error_cause */

  /* If Additional Assistance Data is required */
  boolean addtl_assistance_data_incl;

  /* Additional Assistance Data Required of what type */
  tdsrrc_additional_assistance_data_struct_type addtl_assistance_data;

} tdsrrc_positioning_error_struct_type;


/* A-GPS Measurement Report actual Meas Contents */

typedef PACKED struct PACKED_POST
{
  /* Measurement identity */
  uint8   meas_id;

  /* True indicates that LSM has sent to RRC the maximum number of reports that
  LSM was configured to send for this measurement ID. This is in the case of
  periodic mode reporting criteria. LSM deletes that measurement  after sending this.
  RRC would delete the measurement identity after receiving this indication */
  boolean num_reports_ind;

  /* Indicates if it is a periodic or event trigger only */
  uint8 report_mode; /* tdsrrc_report_crit_enum_type */

  /* Event Trigger structure to be defined later */

  /* Positioning Estimate */
  boolean positioning_estimate_incl;
  tdsrrc_positioning_estimate_struct_type positioning_estimate;

  /* GPS Measured Results */
  boolean gps_measured_results_incl;
  tdsrrc_gps_measured_results_struct_type gps_measured_results;

  /* Positioning Error */
  boolean positioning_error_incl;
  tdsrrc_positioning_error_struct_type pos_error;

  /* Report Transfer Mode. This value is always copied by LSM from below
    while sending Measurement Report to RRC. This value is always specified
    by RRC when it sends the Meas Cmd (SETUP) the first time */
  uint8 tranfer_mode; /* tdsrrc_report_transfer_mode_enum_type */

} tdsrrc_meas_rpt_struct_type;

/* A-GPS Measurement Report Framework i.e RRC_AGPS_MEAS_IND details */

typedef PACKED struct PACKED_POST
{
  tdsrrc_meas_rpt_struct_type *meas_rpt; /* Measurement Report */
} tdsrrc_meas_rpt_ind_type ;


/* A-GPS Response to Measurement Control Message i.e. RRC_MEAS_CTRL_CNF details */

typedef PACKED struct PACKED_POST
{
  uint8 meas_id; /* Meas Identity that was given to AGPS in Meas Ctrl Message */
  uint8 ack_id; /* Set to a value that RRC sets in Meas Ctrl Message. RRC uses this to identify the response */
  boolean status;   /* True indicates that the command was accepted */
  uint8 error_cause; /* tdsrrc_error_cause_enum_type */
} tdsrrc_meas_ctrl_rsp_type;

/* GPS TOW info */

typedef PACKED struct PACKED_POST
{
  uint8 satellite_identity; /* 0 .. 63 */
  /* RRC QUERY: What is TLM Msg used for */
  uint16  tlm_msg;         /* bitstring tlm_msg; 14 bits */
  uint8 tlm_rsvd;    /* bitstring  tlm_recvd 2 bits */
  boolean alert;
  boolean anti_spoof;
} tdsrrc_gps_tow_assist_struct_type;


/* GPS Reference Time */

typedef PACKED struct PACKED_POST
{
  /* GPS Week */
  uint16 week; /* 0..1023 */
  /* GPS Time of Week in milliseconds  */
  uint32 time;  /* 0.. 604799999 */

  /* GPS Reference Time */
  boolean gps_utran_reference_time_incl;
  tdsrrc_gps_umts_reference_time_struct_type gps_utran_ref_time;

  /* Uncertainty of the relation GPS TOW/SFN */
  boolean sfn_tow_uncertainity_incl;
  uint8 sfn_tow_uncertainity; /* rrc_sfn_tow_uncertainty_enum_type */

  /* GPS Drift Rate */
  boolean gps_drift_rate_incl;

  /* 50 ... -50 */
  int8 gps_drift_rate;

  uint8 no_of_gps_tow_assist;

  /* Satellite Information */
  tdsrrc_gps_tow_assist_struct_type  gps_tow_assist[TDSMAX_NO_OF_SATELLITES];

} tdsrrc_gps_reference_time_struct_type;

/* GPS Ephemeris and Clock Correction parameters */

typedef PACKED struct PACKED_POST
{
  /* RRC QUERY: LSM to find out the value conversion required from
  [12]  ICD-GPS-200: "Navstar GPS Space Segment/Navigation User Interface". */

  uint8    code_on_l2; /* BitString 2 */
  uint8    ura_index; /* BitString 4 */
  uint8   sat_health; /* BitString 2 */
  uint16    iodc; /* BitString 1 */
  boolean    l2_pflag; /* BitString 2 */
  uint8    t_gd; /* BitString 8 */
  uint16    t_oc; /* BitString 16 */
  uint8    af2; /* BitString 8 */
  uint16    af1; /* BitString 16 */
  uint32    af0; /* BitString 22 */
  uint16    c_rs; /* BitString 16 */
  uint16    delta_n; /* BitString 16 */
  uint32    m0; /* BitString 32 */
  uint16    c_uc; /* BitString 16 */
  uint32    e; /* BitString 32 */
  uint16    c_us; /* BitString 16 */
  uint32    a_Sqrt; /* BitString 32 */
  uint16    t_oe; /* BitString 16 */
  boolean    fit_interval; /* BitString 1 */
  uint8    aodo; /* BitString 5 */
  uint16    c_ic; /* BitString 16 */
  uint32    omega0; /* BitString 32 */
  uint16    c_is; /* BitString 16 */
  uint32    i0; /* BitString 32 */
  uint16    c_rc; /* BitString 16 */
  uint32    omega; /* BitString 32 */
  uint32    omegaDot; /* BitString 24 */
  uint16    iDot; /* BitString 14 */
} tdsrrc_ephermeral_clock_struct_type;


/* Navigation Information about Satellites */

typedef PACKED struct PACKED_POST
{
  uint8 satellite_id; /* 0 to 63 */

  /* Satellite Status Navigation Model e.g. Same Satellite, New Model */
  uint8 satellite_status; /* tdsrrc_satellite_status_enum_type */

  /* GPS Ephemeris and Clock Correction parameters. */

  boolean ephermeral_clock_params_incl;
  tdsrrc_ephermeral_clock_struct_type ephermeral_clock_params;

} tdsrrc_navigation_model_sat_info_list_struct_type;

/* GPS Navigation Model */

typedef PACKED struct PACKED_POST
{
  uint8 no_of_satellites;
  tdsrrc_navigation_model_sat_info_list_struct_type satellite_info[TDSMAX_NO_OF_SATELLITES];

} tdsrrc_gps_navigation_model_struct_type;


/* GPS Real Time Integrity */

typedef PACKED struct PACKED_POST
{
  uint8 no_of_satellites;
  uint8 satellite_id[TDSMAX_NO_OF_SATELLITES];
} tdsrrc_gps_rti_struct_type;

/* GPS Almanac Info RRC Query: LSM to find out the values */

typedef PACKED struct PACKED_POST
{
  uint8 data_id; /* (0..3) */
  uint8 satellite_id; /* 0 to 63 */

  uint16 e;     /* BIT STRING 16 */
  uint8  t_oa;  /* BIT STRING 8 */
  uint16 deltaI;    /* BIT STRING 16 */
  uint16 omega_dot; /* BIT STRING 16 */
  uint8  sat_health;    /* BIT STRING 8 */
  uint32 a_Sqrt;    /* BIT STRING 24 */
  uint32 omega0 ;/* BIT STRING 24 */
  uint32 m0; /* BIT STRING 24 */
  uint32 omega; /* BIT STRING 24 */
  uint16 af0;   /* BIT STRING 11 */
  uint16 af1;   /* BIT STRING 11 */

} tdsrrc_almanac_sat_info_struct_type;

/* GPS Almanac */

typedef PACKED struct PACKED_POST
{
  /* RRC QUERY */
  uint8 wn_a; /* BitString 8 */

  uint8 no_of_almanac_sat;
  /* Alamanc Info */
  tdsrrc_almanac_sat_info_struct_type almanac_sat_info_list[TDSMAX_NO_OF_SATELLITES];

  boolean global_health_incl;
  uint16 global_health_num_bits;
  uint8 global_health[TDSGLOBAL_HEALTH_ARRAY_SIZE];

} tdsrrc_gps_almanac_struct_type;


/* Extra Doppler Information */

/* Extra Doppler Information */
typedef PACKED struct PACKED_POST
{
  /* LSM Multiplies by .023 to get actual value */
 int32 doppler_first_order; /* (-42..21)  */
 uint8 Doppler_uncertainity; /* rrc_doppler_uncertainty_enum_type */
} tdsrrc_extra_doppler_info_struct_type;


typedef PACKED struct PACKED_POST
{
  /* Recvd OTA as (0..31). Actual Value is 11.25*azimuth. LSM does the multiplication */
  uint32 azimuth; /* Actual Value is 0..348.75 */
  /* Recvd OTA as (0..7). Actual Value is 11.25*elevation. LSM does the multiplication */
  uint32 elevation; /* 0..78.75 by step of 11.25 */
} tdsrrc_azimuth_elevation_struct_type;

/* Satellite Information in GPS Acq Assistance */

typedef PACKED struct PACKED_POST
{
  uint8 satellite_id; /* 0 to 63 */
  /* Actual doppler value is doppler*2.5. LSM multiplies by 2.5 to get the actual value from specified value */
  int32 doppler; /* (-2048..2047) as received OTA */

  /* Extra Doppler Info Present */
  boolean extra_doppler_info_incl;
  tdsrrc_extra_doppler_info_struct_type extra_doppler_info;

  /* Centre of the search window */
  uint32 code_phase; /* 0..1022 */
  uint32 integer_code_phase; /* 0..19 */
  /* GPS Bit Number */
  uint32 gps_bit_number; /* 0..3*/


  /* Code Phase Search Window 1023 is Max value */
  uint16 search_window;

  /* Azimuth and Elevation Present */
  boolean azimuth_elevation_incl;
  tdsrrc_azimuth_elevation_struct_type azimuth_elevation_info;

} tdsrrc_gps_sat_info_struct_type;

/* GPS acquisition assistance. Enable fast acquisition of GPS signals in UE-assisted GPS */
typedef PACKED struct PACKED_POST
{
  /* GPS Reference Time */
  uint32 gps_reference_time;

  /* Utran GPS Reference Time */
  boolean gps_umts_reference_time_incl;
  tdsrrc_gps_umts_reference_time_struct_type gps_umts_reference_time;

  uint8 no_of_gps_sat;
  /* Satellite Information */
  tdsrrc_gps_sat_info_struct_type gps_sat_info[TDSMAX_NO_OF_SATELLITES];
} tdsrrc_gps_acq_assistance_struct_type;


/* GPS Iono */

typedef PACKED struct PACKED_POST
{
  uint8 alfa0;
  uint8 alfa1;
  uint8 alfa2;
  uint8 alfa3;
  uint8 beta0;
  uint8 beta1;
  uint8 beta2;
  uint8 beta3;
} tdsrrc_gps_iono_struct_type;

/* GSP UTC model */

typedef PACKED struct PACKED_POST
{
  uint32 q_A1;
  uint32 q_A0;
  uint8  u_Tot;
  uint8  u_WNt;
  uint8  u_DeltaTls;
  uint8  u_WNlsf;
  uint8  u_DN;
  uint8  u_DeltaTlsf;
} tdsrrc_gps_utc_struct_type;




/* GPS Assistance Data in MCM */

typedef PACKED struct PACKED_POST
{
  /* GPS Reference Time */
  boolean gps_ref_time_incl;
  tdsrrc_gps_reference_time_struct_type gps_ref_time;

  /* GPS Reference Location */
  boolean gps_ref_loc_incl;
  tdsrrc_gps_reference_loc_struct_type gps_ref_loc;

  /* GPS DGPS Corrections. NOT Supported for now. Structure to be added later */

  /* GPS Navigation Model of Satellites Info */
  boolean gps_navigation_model_incl;
  tdsrrc_gps_navigation_model_struct_type gps_navigation_model;

  /* GPS UTC Model */
  boolean gps_utc_model_incl;
  tdsrrc_gps_utc_struct_type gps_utc_model;

  /* GPS Almanac. Reduced precision subset of ephemeris & clock correction params. */
  boolean gps_almanac_incl;
  /* Structure to be added as everything is a almanac */
  tdsrrc_gps_almanac_struct_type gps_almanc;

  /* GPS Ionospheric Model */
  boolean gps_iono_incl;
  tdsrrc_gps_iono_struct_type gps_iono;

  /* GPS Acquisition Assistance*/
  boolean gps_acq_assistance_incl;
  tdsrrc_gps_acq_assistance_struct_type gps_acq_assistance;

  /* GPS Real Time Integrity i.e. Real Time status of the GPS constellation */
  boolean gps_real_time_integrity_incl;
  tdsrrc_gps_rti_struct_type gps_rti;

  /* Reference Cell Info */
  boolean reference_cell_info_incl;
  /* Scrambling Code of the Reference Cell */
  uint8 cpid;

} tdsrrc_assistance_data_struct_type;





/* Reporting Quantity */

typedef PACKED struct PACKED_POST
{
  /* Method Type */
  uint8 method_type; /* tdsrrc_positioning_method_type_enum_type */

  /* Method */
  uint8 method; /* tdsrrc_positioning_method_enum_type */

  /* Horizontal accuracy incl */
  boolean hor_accuracy_incl;

  uint32 horizontal_accuracy;

  /* SFN-GPS timing of the reference cell. */
  boolean sfn_gps_timing_required;

  /* Indicates that UE is requested to send the IE "Additional assistance Data Request"
  when the IE "UE positioning Error" is set in the UE positioning measured results.
  [Measurement Report]*/
  boolean additional_assistance_data_request;

  boolean environ_ch_incl;

  uint8 environ_ch; /* tdsrrc_environment_characterisation_enum_type */

} tdsrrc_rpt_quan_struct_type;

/* Periodic Reporting Structure */

typedef PACKED struct PACKED_POST
{
  uint8 rpt_amount;
    /* Amount of reporting (1,2,4,8,16,32,64, Infinity */
    /* Measurement is "released" after the indicated amount (number) of */
    /* report(s) from the UE itself. 7 means "infinity". As per LSM req, it would do conversion on its own */
  uint8  rpt_interval;
    /* Reporting Interval  0-8. As per LSM req, it would do conversion on its own */
} tdsrrc_periodic_rpt_crit_struct_type;

/* GPS Reporting Criteria */

typedef PACKED struct PACKED_POST
{
  uint8 rpt_mode; /* tdsrrc_report_crit_enum_type */

  PACKED union PACKED_POST  
  {
    /* TBD Later: Event Triggered Measurement reporting criteria */
    /* Periodical Measurement reporting criteria as in 10.3.7.53 */
    tdsrrc_periodic_rpt_crit_struct_type periodic_crit;
  } u;
} tdsrrc_reporting_criteria_struct_type;

/* A-GPS  Meas Control Structure */

typedef PACKED struct PACKED_POST
{
  /* Measurment identity */
  uint8       meas_id;

  /* Meas Command, Start/Modify/Delete */
  uint8 meas_cmd; /* tdsrrc_meas_cmd_enum_type */

  /* True Indicates that RRC is expecting a confirmation message from LSM  */
  boolean     config_conf_ind;

  /* LSM should use this ack number while sending response to RRC in RRC_MEAS_CTRL_CNF */
  uint8       config_ack_num;

  /* Reporting Quantity. Always come with SETUP. Optional with Modify: Then LSM should use
     previous one */
  boolean rpt_quan_incl;

  tdsrrc_rpt_quan_struct_type rpt_quan;


  /* Reporting Criteria. Always come with SETUP. Optional with Modify: Then LSM should use
     previous one */
  tdsrrc_reporting_criteria_struct_type rpt_crit;

  /* GPS Assistance Data Included */
  boolean gps_assistance_data_incl;

  /* GPS Assistance Data */
  tdsrrc_assistance_data_struct_type gps_assistance_data;

  /* Transfer Mode always specifed for a SETUP cmd.
  May not be specifed for a Modify cmd.
  If not specified, then LSM should use the
  previous one */
  boolean transfer_mode_incl;

  /* Report Transfer Mode. This value is always set by LSM while
   sending Measurement Report to RRC */
  uint8 tranfer_mode; /* tdsrrc_report_transfer_mode_enum_type */

  /* Valid for SETUP cmd only. If set, then only measurement should be done. Else wait for a
     RESUME cmd from RRC */
  boolean meas_start_incl;

} tdsrrc_meas_ctrl_struct_type;


/* RRC_LSM_MEAS_REQ structure */

typedef PACKED struct PACKED_POST
{
  uint8 params_type; /* Of type tdsrrc_meas_params_enum_type */

  PACKED union PACKED_POST
  {
    tdsrrc_meas_ctrl_struct_type meas_ctrl_params;
    tdsrrc_assistance_data_struct_type assistance_data_params;
    /* RRC Transition Structure to be defined later */
    tdsrrc_state_trans_struct_type trans_params;
    /* SIB Structure to be defined later */
  } u;
} tdsrrc_meas_params_struct_type;


typedef enum
{
  TDS_CLASSMARK_UE_CAPABILITY_NETWORK_BASED = 0,
  TDS_CLASSMARK_UE_CAPABILITY_UE_BASED,
  TDS_CLASSMARK_UE_CAPABILITY_BOTH,
  TDS_CLASSMARK_UE_CAPABILITY_NONE
} tdsnetwork_assisted_gps_support_enum_type;


typedef struct
{
  boolean standalone_location_method_supported;
  boolean ue_based_otdoa_supported;
  tdsnetwork_assisted_gps_support_enum_type  network_assisted_gps_support;
  boolean gps_timing_of_cell_frames_supported;
  boolean ipdl_supported;
  boolean rx_tx_type2_supported;
  boolean up_meas_validity_cell_pch_and_ura_pch;
  boolean lcs_value_added_lcs_supported;
} tm_umts_cp_tds_ue_pos_capability_struct_type;


typedef enum /* for possible future expansion */
{
  TDSRRC_CGPS_RRC_MSG_EVENT
} tdsrrc_cgps_event_e_type;


typedef struct
{
  uint8*          p_data;
  uint32          length;
} tdsrrc_cgps_msg_data_s_type;

typedef union /* for possible future expansion */
{
  tdsrrc_cgps_msg_data_s_type    z_rrc_cgps_msg_data;
} tdsrrc_cgps_event_data_union_type;

typedef struct
{
  tdsrrc_cgps_event_e_type            e_event_type;
  tdsrrc_cgps_event_data_union_type   u;
} tdsrrc_cgps_event_data_s_type;

typedef boolean (* tdsrrcgps_cgps_event_cb_type ) ( tdsrrc_cgps_event_data_s_type* event_data_ptr );
typedef void (* tdsrrcgps_cgps_ue_pos_capability_cb_type) ( tm_umts_cp_tds_ue_pos_capability_struct_type *out_capabilities_ptr );



typedef struct
{
  uint32 cell_id;
  uint8 cpid;
  uint16 freq;
  rrc_plmn_identity_type plmn_id;
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
} tdsrrcmeas_plmn_cell_info_struct_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum
{
  TDSRRC_NEW_CELL_ID_AVAIL,
  TDSRRC_NO_CELL_ID_AVAIL
}tdsrrc_cell_id_status_type  ;


/*Call Back function will be called in response to TDSRRC_GPS_CELL_ID_REQUEST*/
  typedef void 
TDSRRC_GPS_CELL_ID_CNF_CB_FUNC_TYPE (
   tdsrrcmeas_plmn_cell_info_struct_type  cell_id_info,
   tdsrrc_cell_id_status_type  status 
);


/*===========================================================================

FUNCTION tdsrrcmeas_get_cell_plmn_info

DESCRIPTION

 Returns the PLMN id, Cell id ,Cpid, Freq of the current serving cell.
 If the fn is invoked in INITIAL or DCH substate of measurement
 procedure, then false is returned indicating that cell id is not
 available

DEPENDENCIES
 None

RETURN VALUE

 True if Cell Id/PLMN/FREQ/SCR code available
 False if Cell Id/PLMN/FREQ/SCR code unavailable

SIDE EFFECTS

 None

===========================================================================*/

boolean tdsrrcmeas_get_cell_plmn_info
(
  tdsrrcmeas_plmn_cell_info_struct_type* plmn_cell_info_ptr
);


/* defined in tdsrrcgpsmeas.c, for CGPS_UMTS_WCDMA to register the RRC event
   call back function */
boolean tdsrrcgps_register_cgps_event_cb ( tdsrrcgps_cgps_event_cb_type event_cb_fp );


/* defined in tdsrrcgpsmeas.c, for CGPS_UMTS_CP_WCDMA to register the RRC UE
   capability call back function */
boolean tdsrrcgps_register_cgps_ue_pos_capability_cb ( tdsrrcgps_cgps_ue_pos_capability_cb_type ue_cap_cb_fp);


/*===========================================================================

FUNCTION  tdsrrc_register_cell_id_cnf_cb

DESCRIPTION

  This function registers the callback function to be called for the returning the cell id values to GPS.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE if Registration is success
  else
  FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_register_cell_id_cnf_cb
(
  TDSRRC_GPS_CELL_ID_CNF_CB_FUNC_TYPE  *cb_ptr
);

/*===========================================================================

FUNCTION  tdsrrc_check_tds_in_oos

DESCRIPTION

  This function is called by GPS to check if TDS searching is in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If W search is in progress
  else
  FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_check_tds_in_oos
(
  void
);



/*===========================================================================

FUNCTION rrcscr_get_signalling_connection_status

DESCRIPTION
  This function gets signalling Connections to release status of Domain.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is an active Signalling connection. Otherwise it returns
  FALSE

===========================================================================*/

boolean rrcscr_get_signalling_connection_status
(
  rrc_cn_domain_identity_e_type cn_domain_id                   
);

#endif  /* TDSRRCLSMIF_H */


