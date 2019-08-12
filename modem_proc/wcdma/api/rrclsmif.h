#ifndef RRCLSMIF_H
#define RRCLSMIF_H

/*========================================================================================

   RRC - LSM INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by RRC and LSM

  Copyright (c) 2003-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrclsmif.h#1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
01/24/14    vi      Enabled rrc_mulitpath_enum_type
10/09/13    geg     Clean up WCDMA private API from interface inclusion chain for Feature Segment Loading DIME 3.0
09/11/13    vi      Declaring reference time as rrc_positioning_estimate_enum_type
08/26/13    bc      Added changes to indicate the current rrc version when the respective api is called.
06/22/13    sr      Changes to fix compilation errors
06/20/13    sr      Adding AGNSS RRC R8 changes without feature flag in API
05/23/13    geg     WSW decoupling (except for component of QCHAT)
05/13/13    vi      Added code for GANSS under feature FEATURE_GANSS_SUPPORT
07/29/11    ad      Made changes for CMI Phase 4
02/10/11    rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11    ad      Added changes to refeaturize the API files
01/28/11    rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11    ad      Added changes for RRC SW decoupling
10/30/10    rl      Objective systems migration changes check in 
                    on code base VU_MODEM_WCDMA_RRC.01.80.00
08/24/10    rm      Added GPS reference time uncertainity field to 
                    rrc_gps_measured_results_struct_type
08/11/09    rm      Removed the dependency from comdefs.h
07/20/07    rm      Moved definition of rrcmeas_plmn_cell_info_struct_type out of WIN32 feature
07/14/09    rm      Added LAC info to rrcmeas_plmn_cell_info_struct_type
05/13/09    rm      Added an API for GPS to check if W search is in progress.
05/08/09    ss      Updated Copyright Information
09/12/08    ps      Added code for REl7 under FEATURE_WCDMA_REL7
02/06/08    rm      Made changes to rename variables in rrc_rpt_quan_struct_type
08/29/07    ps      Moved the RRC GPS procedure interface code changes  from rrcgps.h 
                    to rrclsmif.h. Changes are made under feature flag 
                    FEATURE_WCDMA_GPS_CELL_ID_READING.
05/25/07    vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
05/11/06    sgk     Added support for lsm pch reporting, included 2 new cmds
                    RRC_CHANGE_PERIODIC_INTERVAL_TO_PCH to change Periodic Reporting Interval
                    to 64 secs and RRC_CHANGE_PERIODIC_INTERVAL_FROM_PCH to move back the interval
                    to the earlier value configured for other states.
03/03/06    da      Backed out changes made to temporarily resolve LSM compilation issues.
03/03/06    vg      Temporarily added to resolve LSM files using pre-REL6 asn variables
05/06/05    vk      Declared new structure rrc_gps_rti_struct_type for GPS Real Time Integrity
                    and rrc_gps_utc_struct_type for GPS UTC Model. These structures are used
                    by LSM
04/05/05    vk      Declared function rrc_lsm_meas_report_notification that indicates to LSM
                    if the AGPS meas report was rcvd by the network under
                    FEATURE_AAGPS_MR_RRC_NOTIFICATION_REQUIRED
11/11/04    vk      Moved meas_start_incl variable to the end of the rrc_meas_ctrl_struct_type
                    structure so that SNAPPER can parse the structure and display the meas request
10/15/04    vk      Added new primitive to indicate if measurement needs to be started on receiving
                    the configuration request from RRC
02/02/04    vk      Initial release that contains interface between LSM and RRC

========================================================================================*/



#include "rrcdata.h"
#include "rrcmmif.h"

#define MAX_NO_OF_SATELLITES 16
#define GLOBAL_HEALTH_ARRAY_SIZE 46

#define MAX_GANSS 1
#define MAX_SGN_TYPE 1
#define MAX_GANSS_SAT 24
#define MAX_POS_DATA 2


/* Type of parameters */
typedef enum
{
  /* Measurement parameters from SIB 11/12 */
  RRC_SIB_PARMS,
  /* Measurement parameters from Measurement Control Message */
  RRC_MEAS_CTRL_PARMS,
   /* Measurement parameters from Assistance Data Delivery*/
  RRC_ASSISTANCE_DATA_PARMS,
  /* Measurement parameters from RRC on state transition from DCH */
  RRC_STATE_CHANGE_PARMS
} rrc_meas_params_enum_type;


/* Command Action */
typedef enum
{
  RRC_MEAS_SETUP = 1,  /* Start A-GPS Measurements  */
  RRC_MEAS_MODIFY,     /* Modify an existing measurement  */
  RRC_MEAS_DELETE,     /* Delete meas */
  RRC_MEAS_DELETE_ALL, /* Deletes all the existing measurement ids   */
  RRC_MEAS_SAVE,       /* Stop & Save the measurement info */
  RRC_MEAS_RESUME,      /* Resume measurement */
  RRC_CONTINUE_CHANGE_PERIODIC_INTERVAL,
  RRC_CHANGE_PERIODIC_INTERVAL_TO_PCH,   /* Change Periodic Reporting Interval to 64 secs */
  RRC_CHANGE_PERIODIC_INTERVAL_FROM_PCH /* change back from 64s to the earlier stored value */
} rrc_meas_cmd_enum_type;

/* Positioning Method Type */
typedef enum
{
  RRC_UE_ASSISTED = 0,
  RRC_UE_BASED,
  RRC_UE_BASED_PREFERED,
  RRC_UE_ASSISTED_PREFERED
} rrc_positioning_method_type_enum_type;

/* Positioning Method */
typedef enum
{
  RRC_OTDOA = 0,
  RRC_GPS,
  RRC_OTDOAorGPS,
  RRC_CELLID
} rrc_positioning_method_enum_type;

/* Navigation Model of Satellite */
typedef enum
{
  RRC_NEW_SATELLITE__NEW_NAVIFGATION_MODEL = 0,
  RRC_EXISTING_SATELLITE_SAME_NAVIGATION_MODEL,
  RRC_EXISTING_SATELLITE_NEW_NAVIGATION_MODEL
} rrc_satellite_status_enum_type;


/* Doppler Uncertainty in Hz */
typedef enum
{
   RRC_HZ12_5 = 0, /* 12.5 */
   RRC_HZ25, /* 25 */
   RRC_HZ50, /* 50 */
   RRC_HZ100, /* 100 */
   RRC_HZ200 /* 200 */
} rrc_doppler_uncertainity_enum_type;

/* Error Cause */
typedef enum
{
  /* LSM sets it if it finds invalid configuration for
     A-GPS Meas with actual Meas Params */
  CONFIGURATION_INVALID
} rrc_error_cause_enum_type;



/* Positioning Estimate is based on this type */
typedef enum
{
  UTRAN_GPS_REFERENCE_TIME = 1,
  GPS_REFERENCE_TIME_ONLY,
  CELL_TIMING_ONLY,
  GANSS_REFERENCE_TIME_ONLY
} rrc_positioning_estimate_enum_type;


/* SFN-TOW Uncertainty. LessThan10 means the relation is accurate to at least 10 ms.*/
typedef enum
{
  LESS_THAN_10,
  MORE_THAN_10
} rrc_sfn_tow_uncertainity_enum_type;

/* SFN-TOW Uncertainty. LessThan10 means the relation is accurate to at least 10 ms.*/
typedef enum
{
  NORTH,
  SOUTH
} rrc_latitude_sign_enum_type;

/* Altitude Direction.*/
typedef enum
{
  HEIGHT,
  DEPTH
} rrc_altitude_direction_enum_type;

/* Type of Reporting */
typedef enum
{
  RRC_EVENT_TRIGGER_RPT = 1,
  RRC_PERIODIC_RPT,
  RRC_NO_RPT /* Additional Measurement */
} rrc_report_crit_enum_type;



/* Report Transfer Mode */
typedef enum
{
  MODE_ACKNOWLEDGED,
  MODE_UNACKNOWLEDGED
} rrc_report_transfer_mode_enum_type;


/* Envrionment Ch */
typedef enum
{
  POSSIBLE_HEAVY_MULTIPATH_LOS,
  LIGHT_MULTIPATH_LOS
} rrc_environment_characterisation_enum_type;

/* Positioning Error Codes used by RRC and LSM */
typedef enum
{
  RRC_NOT_ENOUGH_OTDOA_CELLS = 0,
  RRC_NOT_ENOUGH_GPS_SATELLITES,
  RRC_ASSISTANCE_DATA_MISSING,
  RRC_METHOD_NOT_SUPPORTED,
  RRC_UNDEFINED_ERROR,
  RRC_REQUEST_DENIED_BY_USER,
  RRC_NOT_PROCESSED_AND_TIMEOUT,
  RRC_REFERENCE_CELL_NOT_SERVING_CELL,
  RRC_NOT_ENOUGH_GANSS_SATELLITES,
  RRC_NOT_ACCOMPLISHED_GANSS_TIMING_OF_CELLFRAMES
} rrc_positioning_error_cause;


/* Multipath Type */
typedef enum
{
  RRC_GPS_NOT_MEASURED,  /*   Not measured */
  RRC_GPS_LOW,           /* Multipath error < 5m */
  RRC_GPS_MEDIUM,        /* 5m < Multipath error < 43m */
  RRC_GPS_HIGH           /* Multipath error > 43m */
} rrc_mulitpath_enum_type;

typedef enum
{
  ELLIPSOID_POINT = 1,
  ELLIPSOID_POINT_UNCERTAIN_CIRCLE,
  ELLIPSOID_POINT_UNCERTAIN_ELLIPSE,
  ELLIPSOID_POINT_ALTITUDE,
  ELLIPSOID_POINT_ALTITUDE_ELLIPSE
} rrc_pos_estimate_result_enum_type;

typedef enum
{
  RRC_GPS_SYSTEM_TIME,
  RRC_QZSS_SYSTEM_TIME,
  RRC_GLONASS_SYSTEM_TIME,
  RRC_RESERVED_TIME_ID
}rrc_gANSS_timeId_enum_type;

typedef enum
{
  HORIZONTAL_VELOCITY = 1,
  HORIZONTAL_WITH_VERTICAL_VELOCITY,
  HORIZONTAL_VELOCITY_WITH_UNCERTAINTY,
  HORIZONTAL_WITH_VERTICAL_VELOCITY_AND_UNCERTAINTY
} rrc_velocity_estimate_enum_type;
typedef enum
{
  UPWARD,
  DOWNWARD
}rrc_vertical_speed_direction_enum_type;

typedef PACKED struct PACKED_POST
{
  uint16 bearing;
  uint16 horizontalSpeed;
  uint8 verticalSpeedDirection; /*rrc_vertical_speed_direction_enum_type*/
  uint16 verticalSpeed;
  uint16 horizontalSpeedUncertainty;
  uint16 horizontalUncertaintySpeed;
  uint16 verticalUncertaintySpeed;
}rrc_velocity_estimate_type_info;
typedef PACKED struct PACKED_POST
{
  uint8 velocity_estimate_choice;  /*rrc_velocity_estimate_enum_type*/
  rrc_velocity_estimate_type_info velocity_type;

}rrc_velocityEstimate_struct_type;

/* State Transition Command */

typedef PACKED struct PACKED_POST
{
  /* Here Cmd Type can only 5 values i.e.
  RRC_MEAS_DELETE,
  RRC_MEAS_DELETE_ALL,
  RRC_MEAS_SAVE,
  RRC_MEAS_RESUME,
  RRC_CONTINUE_CHANGE_PERIODIC_INTERVAL     */
  uint8 trans_cmd_type; /* Of rrc_meas_cmd_enum_type */
  uint8 meas_id;
} rrc_state_trans_struct_type;

/* Cell Timing Only */
typedef PACKED struct PACKED_POST
{
  /* SFN of current cell */
  uint32 sfn;
  /* Primary Scrambling Code of current cell */
  uint16 psc;

} rrc_cell_timing_struct_type;

/* GPS UMTS Reference Time */

typedef PACKED struct PACKED_POST
{
  /* UTRAN GPS timing of cell frames in steps of 1 chip */

  /* Actual Value for UTRAN GPS timing is (msb_timing * 4294967296) + lsb_timing */
  /* msb_timing (0..1023),lsb_timing (0..4294967295) */
  uint16 msb_timing;
  uint32 lsb_timing;


  boolean  gps_ref_psc_incl;
  /* Reference cell for the GPS TOW-SFN relationship */
  uint16 psc; /* 0 .. 511 */

  /* SFN at which the UTRAN GPS timing of cell frames time stamps */
  uint32 sfn; /* 0..4095 */

} rrc_gps_umts_reference_time_struct_type;

/* GPS Reference Location i.e. Ellipsoid point with altitude and uncertainty ellipsoid */
typedef PACKED struct PACKED_POST
{
  /* North or South */
  uint8 latitude_sign; /* Of rrc_latitude_sign_enum_type */

  /* RRC QUERY pending for the following values, waiting for clarification */

  /* Latitude in Degrees*/
  uint32 latitude; /* 0...223-1     N 223 X /90   N+1 (0..8388607) */

  /* Longitude in Degrees*/
  int32 longitude; /* 0...223-1    N 224 X /360   N+1 (-8388608..8388607)*/

  /* Altitude Direction */
  uint8 altitude_direction; /* Height or Depth, rrc_altitude_direction_enum_type */

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

} rrc_gps_reference_loc_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 satId;
  uint8 cSurNzero;
  uint8 multipathIndicator;  /*rrc_mulitpath_enum_type*/
  uint32 ganssCodePhase;

  /* If ganss Integer Code Phase is required */
  boolean ganssIntegerCodePhase_incl;
  uint8 ganssIntegerCodePhase;

  uint8 codePhaseRmsError;
  int32 doppler;

  /* If ganss Integer Code Phase Ext is required */
  boolean ganssIntegerCodePhaseExt_incl;
  uint8 ganssIntegerCodePhaseExt;
}rrc_ganssMeasurementParameters_struct_type;
typedef PACKED struct PACKED_POST
{
  /* If ganss Signal Id is required */
  boolean ganssSignalId_incl;
  uint8 ganssSignalId;

  /* If ganss Code Phase Ambiguity is required */
  boolean ganssCodePhaseAmbiguity_incl;
  uint8 ganssCodePhaseAmbiguity;

  /* If ganss Code Phase Ambiguity Ext is required */
  boolean ganssCodePhaseAmbiguityExt_incl;
  uint8 ganssCodePhaseAmbiguityExt;

  uint8 meas_param_cnt;
  rrc_ganssMeasurementParameters_struct_type ganssMeasurementParameters[MAX_GANSS_SAT];

  /*GANSSMeasurementParameters-v860ext*/
  /*uint8 meas_param_ext_cnt;
  rrc_ganssMeasurementParameters_struct_type ganssMeasurementParameters_Ext[MAX_GANSS_SAT];*/
}rrc_ganssMeasurementSignalList_struct_type;
typedef PACKED struct PACKED_POST
{
  uint32 gANSS_tod;
  boolean gANSS_timeId_incl;
  uint8 gANSS_timeId;              /*rrc_gANSS_timeId_enum_type*/
  boolean gANSS_tod_uncertainty_incl;
  uint8 gANSS_tod_uncertainty;
}rrc_ganssReferenceTimeOnly_struct_type;

typedef enum
{
  RRC_SBAS = 0,
  RRC_MODERNIZED_GPS,
  RRC_QZSS,
  RRC_GLONASS,
  RRC_SPARE4,
  RRC_SPARE3,
  RRC_SPARE2,
  RRC_SPARE1
} rrc_ganss_id_e_type;

typedef PACKED struct PACKED_POST
{
  /* If ganss Id is required */
  boolean ganssId_incl;
  rrc_ganss_id_e_type ganssId;                  /*rrc_gANSS_ID_enum_type*/
  uint8 list_cnt;
  rrc_ganssMeasurementSignalList_struct_type ganssMeasurementSignalList[MAX_SGN_TYPE];

  /*GANSSMeasurementSignalList-v860ext*/
  /*uint8 list_ext_cnt;
  rrc_ganssMeasurementSignalList_struct_type ganssMeasurementSignalList_Ext[MAX_SGN_TYPE];*/

}rrc_ganss_GenericMeasurementInfo_struct_type;
typedef PACKED struct PACKED_POST
{
  rrc_positioning_estimate_enum_type referenceTime;  /*rrc_positioning_estimate_enum_type*/
  PACKED union PACKED_POST
  {
    /*Not supported*/
    //rrc_utran_GANSSReferenceTimeResult_struct_type utran_GANSSReferenceTimeResult;

    rrc_ganssReferenceTimeOnly_struct_type ganss_ReferenceTimeOnly;

  }u;
  uint8 meas_cnt;
  rrc_ganss_GenericMeasurementInfo_struct_type ganssGenericMeasurementInfo[MAX_GANSS];

  /*GANSSGenericMeasurementInfo-v860ext*/
  /*uint8 meas_ext_cnt;
  rrc_ganss_GenericMeasurementInfo_struct_type ganssGenericMeasurementInfo_Ext[MAX_GANSS];*/
}rrc_ganss_measured_results_struct_type;


/* Positioning Estimate */
typedef PACKED struct PACKED_POST
{
  rrc_positioning_estimate_enum_type positioning_estimate; /* rrc_positioning_estimate_enum_type */
  PACKED union PACKED_POST
  {
    /* GPS Reference Time Only */
    uint32 gps_reference_time;

    /* Cell Timing Only */
    rrc_cell_timing_struct_type cell_timing;

    /* UTRAN GPS Reference Time */
    rrc_gps_umts_reference_time_struct_type utran_gps_ref_time;

    rrc_ganssReferenceTimeOnly_struct_type ganss_ReferenceTimeOnly;

  } u;

  uint8 pos_estimate_choice; /* rrc_pos_estimate_result_enum_type */

  rrc_gps_reference_loc_struct_type gps_ref_location;
  uint8 positionData[MAX_POS_DATA];  /* MSB 1 is set for GPS..and MSB 6 is set for GLONASS*/
  /*Velocity estimate is required*/
  boolean velocityEstimate_incl;
  rrc_velocityEstimate_struct_type velocityEstimate;
} rrc_positioning_estimate_struct_type;




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

} rrc_gps_measured_param_list_struct_type;


/* GPS UMTS Reference Time Results */

typedef PACKED struct PACKED_POST
{
  /* GPS UTRAN reference Time */
  rrc_gps_umts_reference_time_struct_type gps_umts_reference_time;

} rrc_gps_umts_ref_time_meas_res_struct_type;

/* GPS Measured Results */
typedef PACKED struct PACKED_POST
{
  /* Value can only be GPS reference tIme or GPS-UTRAN refernece Time */
  rrc_positioning_estimate_enum_type positioning_estimate; /* rrc_positioning_estimate_enum_type */

  PACKED union PACKED_POST
  {
    /* GPS Reference Time Only */
    uint32 gps_reference_time;

    /* UTRAN GPS Reference Time */
    rrc_gps_umts_ref_time_meas_res_struct_type gps_umts_meas_results;

  } u;

  /* No of satellites */
  uint8 no_of_satellites;

  /* GPS Reference Time uncertainity */
  boolean gps_reference_time_uncertainity_incl;
  uint8 gps_reference_time_uncertainity;

  /* GPS Measured Param List for each satellite */
  rrc_gps_measured_param_list_struct_type gps_measured_params[MAX_NO_OF_SATELLITES];

} rrc_gps_measured_results_struct_type;

/* Satellite related data */
typedef PACKED struct PACKED_POST
{
  /* Satellite Identity */
  uint8 sat_id; /* 0 to 63 */

  /* Issue of Data Ephemeris */
  uint8 iode; /* 0 to 255 */

} rrc_gps_satellite_related_data_struct_type;



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
  rrc_gps_satellite_related_data_struct_type sat_data[MAX_NO_OF_SATELLITES];

} rrc_gps_nav_data_struct_type;

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
  rrc_gps_nav_data_struct_type navModelAddDataRequest;

} rrc_additional_assistance_data_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 satId;
  uint8 iod;
}rrc_ganss_satellite_related_data_struct_type;
typedef PACKED struct PACKED_POST
{
  uint16 ganssWeek;
  uint8 ganssToe;
  uint8 t_toeLimit;
  boolean satellitesListRelatedDataList_incl;
  uint8 sat_cnt;
  rrc_ganss_satellite_related_data_struct_type satellitesListRelatedDataList[MAX_GANSS_SAT];
}rrc_ganssNavigationModelAdditionalData_struct_type;
typedef PACKED struct PACKED_POST
{
  /* If ganssId is required */
  boolean ganssId_incl;
  rrc_ganss_id_e_type ganssId;

  boolean ganssRealTimeIntegrity;

  boolean ganssDifferentialCorrection_incl;
  uint8 ganssDifferentialCorrection;

  boolean ganssAlmanac;
  boolean ganssNavigationModel;

  boolean ganssTimeModelGNSS_GNSS_incl;
  uint8 ganssTimeModelGNSS_GNSS;

  boolean ganssReferenceMeasurementInfo;
  boolean ganssUTCModel;

  /* If Navigation Model Additional Data is required */
  boolean ganssNavigationModelAdditionalData_incl;
  rrc_ganssNavigationModelAdditionalData_struct_type ganssNavigationModelAdditionalData;

  /* If sbasId is required */
  boolean sbasId_incl;
  uint8 sbasId;

  /* If ganss Add Navigation Model is required */
  boolean ganssAddNavigationModel_incl;


  /* If ganss Add UTC model is required */
  boolean ganssAddUTCmodel_incl;


  /* If ganss Aux Info is required */
  boolean ganssAuxInfo_incl;


  /* If orbit Mode lID is required */
  boolean orbitModelID_incl;
  uint8 orbitModelID;

  /* If clock Model ID is required */
  boolean clockModelID_incl;
  uint8 clockModelID;

  /* If utc Model ID is required */
  boolean utcModelID_incl;
  uint8 utcModelID;

  /* If almanac Model ID is required */
  boolean almanacModelID_incl;
  uint8 almanacModelID;
}rrc_ganssRequestedGenericAssistanceDataList_struct_type;
typedef PACKED struct PACKED_POST
{
  boolean ganssReferenceTime;
  boolean ganssreferenceLocation;
  boolean ganssIonosphericModel;

  /*ganssAddIonoModelReq is required*/
  boolean ganssAddIonoModelReq_incl;
  uint8 ganssAddIonoModelReq;
  uint8 list_cnt;
  rrc_ganssRequestedGenericAssistanceDataList_struct_type ganss_Req_gen_assis_Data[MAX_GANSS];
}rrc_ganss_additional_assistance_data_struct_type;


/* Positioning Error */

typedef PACKED struct PACKED_POST
{
  /* Positioning Error Cause */
  uint8 error_cause; /* rrc_positioning_error_cause */

  /* If Additional Assistance Data is required */
  boolean addtl_assistance_data_incl;

  /* Additional Assistance Data Required of what type */
  rrc_additional_assistance_data_struct_type addtl_assistance_data;
  /* If Additional Assistance Data is required */
  boolean ganss_addtl_assistance_data_incl;

  /* Additional Assistance Data Required of what type */
  rrc_ganss_additional_assistance_data_struct_type ganss_addtl_assistance_data;

} rrc_positioning_error_struct_type;

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
  uint8 report_mode; /* rrc_report_crit_enum_type */

  /* Event Trigger structure to be defined later */

  /* Positioning Estimate */
  boolean positioning_estimate_incl;
  rrc_positioning_estimate_struct_type positioning_estimate;

  /* GPS Measured Results */
  boolean gps_measured_results_incl;
  rrc_gps_measured_results_struct_type gps_measured_results;

  /* Positioning Error */
  boolean positioning_error_incl;
  rrc_positioning_error_struct_type pos_error;

  /* Report Transfer Mode. This value is always copied by LSM from below
    while sending Measurement Report to RRC. This value is always specified
    by RRC when it sends the Meas Cmd (SETUP) the first time */
  uint8 tranfer_mode; /* rrc_report_transfer_mode_enum_type */
  /*GANSS-Measured Results*/
  boolean ganss_measured_results_incl;
  rrc_ganss_measured_results_struct_type ganss_measured_results;

} rrc_meas_rpt_struct_type;

/* A-GPS Measurement Report Framework i.e RRC_AGPS_MEAS_IND details */

typedef PACKED struct PACKED_POST
{
  rrc_meas_rpt_struct_type *meas_rpt; /* Measurement Report */
} rrc_meas_rpt_ind_type ;


/* A-GPS Response to Measurement Control Message i.e. RRC_MEAS_CTRL_CNF details */

typedef PACKED struct PACKED_POST
{
  uint8 meas_id; /* Meas Identity that was given to AGPS in Meas Ctrl Message */
  uint8 ack_id; /* Set to a value that RRC sets in Meas Ctrl Message. RRC uses this to identify the response */
  boolean status;   /* True indicates that the command was accepted */
  uint8 error_cause; /* rrc_error_cause_enum_type */
} rrc_meas_ctrl_rsp_type;

/* GPS TOW info */

typedef PACKED struct PACKED_POST
{
  uint8 satellite_identity; /* 0 .. 63 */
  /* RRC QUERY: What is TLM Msg used for */
  uint16  tlm_msg;         /* bitstring tlm_msg; 14 bits */
  uint8 tlm_rsvd;    /* bitstring  tlm_recvd 2 bits */
  boolean alert;
  boolean anti_spoof;
} rrc_gps_tow_assist_struct_type;


/* GPS Reference Time */

typedef PACKED struct PACKED_POST
{
  /* GPS Week */
  uint16 week; /* 0..1023 */
  /* GPS Time of Week in milliseconds  */
  uint32 time;  /* 0.. 604799999 */

  /* GPS Reference Time */
  boolean gps_utran_reference_time_incl;
  rrc_gps_umts_reference_time_struct_type gps_utran_ref_time;

  /* Uncertainty of the relation GPS TOW/SFN */
  boolean sfn_tow_uncertainity_incl;
  uint8 sfn_tow_uncertainity; /* rrc_sfn_tow_uncertainty_enum_type */

  /* GPS Drift Rate */
  boolean gps_drift_rate_incl;

  /* 50 ... -50 */
  int8 gps_drift_rate;

  uint8 no_of_gps_tow_assist;

  /* Satellite Information */
  rrc_gps_tow_assist_struct_type  gps_tow_assist[MAX_NO_OF_SATELLITES];

} rrc_gps_reference_time_struct_type;

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
} rrc_ephermeral_clock_struct_type;


/* Navigation Information about Satellites */

typedef PACKED struct PACKED_POST
{
  uint8 satellite_id; /* 0 to 63 */

  /* Satellite Status Navigation Model e.g. Same Satellite, New Model */
  uint8 satellite_status; /* rrc_satellite_status_enum_type */

  /* GPS Ephemeris and Clock Correction parameters. */

  boolean ephermeral_clock_params_incl;
  rrc_ephermeral_clock_struct_type ephermeral_clock_params;

} rrc_navigation_model_sat_info_list_struct_type;

/* GPS Navigation Model */

typedef PACKED struct PACKED_POST
{
  uint8 no_of_satellites;
  rrc_navigation_model_sat_info_list_struct_type satellite_info[MAX_NO_OF_SATELLITES];

} rrc_gps_navigation_model_struct_type;


/* GPS Real Time Integrity */

typedef PACKED struct PACKED_POST
{
  uint8 no_of_satellites;
  uint8 satellite_id[MAX_NO_OF_SATELLITES];
} rrc_gps_rti_struct_type;

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

} rrc_almanac_sat_info_struct_type;

/* GPS Almanac */

typedef PACKED struct PACKED_POST
{
  /* RRC QUERY */
  uint8 wn_a; /* BitString 8 */

  uint8 no_of_almanac_sat;
  /* Alamanc Info */
  rrc_almanac_sat_info_struct_type almanac_sat_info_list[MAX_NO_OF_SATELLITES];

  boolean global_health_incl;
  uint16 global_health_num_bits;
  uint8 global_health[GLOBAL_HEALTH_ARRAY_SIZE];

} rrc_gps_almanac_struct_type;


/* Extra Doppler Information */

/* Extra Doppler Information */
typedef PACKED struct PACKED_POST
{
  /* LSM Multiplies by .023 to get actual value */
 int32 doppler_first_order; /* (-42..21)  */
 uint8 Doppler_uncertainity; 
} rrc_extra_doppler_info_struct_type;


typedef PACKED struct PACKED_POST
{
  /* Recvd OTA as (0..31). Actual Value is 11.25*azimuth. LSM does the multiplication */
  uint32 azimuth; /* Actual Value is 0..348.75 */
  /* Recvd OTA as (0..7). Actual Value is 11.25*elevation. LSM does the multiplication */
  uint32 elevation; /* 0..78.75 by step of 11.25 */
} rrc_azimuth_elevation_struct_type;

/* Satellite Information in GPS Acq Assistance */

typedef PACKED struct PACKED_POST
{
  uint8 satellite_id; /* 0 to 63 */
  /* Actual doppler value is doppler*2.5. LSM multiplies by 2.5 to get the actual value from specified value */
  int32 doppler; /* (-2048..2047) as received OTA */

  /* Extra Doppler Info Present */
  boolean extra_doppler_info_incl;
  rrc_extra_doppler_info_struct_type extra_doppler_info;

  /* Centre of the search window */
  uint32 code_phase; /* 0..1023 */
  uint32 integer_code_phase; /* 0..127*/
  /* GPS Bit Number */
  uint32 gps_bit_number; /* 0..3*/


  /*Bitstring(5)*/
  uint16 search_window;

  /* Azimuth and Elevation Present */
  boolean azimuth_elevation_incl;
  rrc_azimuth_elevation_struct_type azimuth_elevation_info;

} rrc_gps_sat_info_struct_type;

/* GPS acquisition assistance. Enable fast acquisition of GPS signals in UE-assisted GPS */
typedef PACKED struct PACKED_POST
{
  /* GPS Reference Time */
  uint32 gps_reference_time;

  /* Utran GPS Reference Time */
  boolean gps_umts_reference_time_incl;
  rrc_gps_umts_reference_time_struct_type gps_umts_reference_time;

  uint8 no_of_gps_sat;
  /* Satellite Information */
  rrc_gps_sat_info_struct_type gps_sat_info[MAX_NO_OF_SATELLITES];
} rrc_gps_acq_assistance_struct_type;


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
} rrc_gps_iono_struct_type;

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
} rrc_gps_utc_struct_type;


typedef enum
{
  LESS_THEN_1 = 0,
  BET_1_AND_4,
  BET_4_AND_8,
  OVER_8
} rrc_udre_e_type;

typedef PACKED struct PACKED_POST
{
  uint16 sat_id;
  uint16 iode;
  rrc_udre_e_type udre;
  int prc;
  int rrc;
}rrc_dgps_correction_sat_Info_type;

typedef enum
{
  RRC_UDRE_1_0 = 0,
  RRC_UDRE_0_75,
  RRC_UDRE_0_5,
  RRC_UDRE_0_3,
  RRC_UDRE_0_2,
  RRC_UDRE_0_1,
  RRC_NO_DATA,
  RRC_INVALID_DATA
}rrc_status_health_e_type;

typedef PACKED struct PACKED_POST
{
  uint32 gps_tow;
  rrc_status_health_e_type status_health;
  rrc_dgps_correction_sat_Info_type dgps_sat_info_list[MAX_GANSS_SAT];
}rrc_gps_dgps_correction_struct_type;

typedef PACKED struct PACKED_POST
{
  boolean ref_time_incl;
  uint8 ref_time;

  boolean acq_assitance_incl;
  uint8 acq_assitance;

}rrc_gps_ref_time_Uncertainty_struct_type;


typedef PACKED struct PACKED_POST
{
  uint32 timing_Of_Cell_Frames;
  uint16 fdd_psc;
  uint32 reference_Sfn;

}rrc_utran_ganssreference_time_struct_type;

typedef PACKED struct PACKED_POST
{
  /* Ganss day */
  boolean day_incl;
  uint32 day; /* 0..1023 */
  /* GPS Time of Week in milliseconds  */
  uint32 ganss_tod;  /* 0.. 604799999 */

  /* GPS Reference Time */
  boolean ganss_Time_Id_incl;
  uint8 ganss_Time_Id;

  /* Uncertainty of the relation GPS TOW/SFN */
  boolean utran_ganss_ref_time_incl;
  rrc_utran_ganssreference_time_struct_type utran_ganss_ref_time; /* rrc_sfn_tow_uncertainty_enum_type */

  /* GPS Drift Rate */
  boolean tutran_ganss_driftRate_incl;  /*Not supported*/
  float tutran_ganss_driftRate;

}rrc_ganss_reference_time_struct_type;

typedef enum
{
  GANSS_ID_GPS = 0,
  GANSS_ID_GALILEO,
  GANSS_ID_QZSS,
  GANSS_ID_GLONASS
} rrc_ganss_to_id_e_type;

typedef PACKED struct PACKED_POST
{
  uint16 ganss_model_ref_time;
  int ganss_t_a0;

  boolean ganss_t_a1_incl;
  int ganss_t_a1;

  rrc_ganss_to_id_e_type ganss_to_id;

  boolean ganss_wk_number_incl;
  uint16 ganss_wk_number;
}rrc_ganss_time_mode_list_struct_type;

typedef PACKED struct PACKED_POST
{

  uint16 cnavToc;
  uint16 cnavTop;
  uint8 cnavURA0;
  uint8 cnavURA1;
  uint8 cnavURA2;
  uint16 cnavAf2;
  uint32 cnavAf1;
  uint32 cnavAf0;
  uint16 cnavTgd;

  boolean cnavISCl1cp_inc;
  uint16 cnavISCl1cp;
  boolean cnavISCl1cd_inc;
  uint16 cnavISCl1cd;
  boolean cnavISCl1ca_inc;
  uint16 cnavISCl1ca;
  boolean cnavISCl2c_inc;
  uint16 cnavISCl2c;
  boolean cnavISCl5i5_inc;
  uint16 cnavISCl5i5;
  boolean cnavISCl5q5_inc;
  uint16 cnavISCl5q5;
}rrc_cnav_Clock_Model_type;

typedef PACKED struct PACKED_POST
{
  uint16 navToc;
  uint8 navaf2;
  uint16 navaf1;
  uint32 navaf0;
  uint8 navTgd;
}rrc_nav_clock_model_type;

typedef PACKED struct PACKED_POST
{
  uint32 gloTau;
  uint16 gloGamma;

  boolean gloDeltaTau_incl;
  uint8 gloDeltaTau;

}rrc_glonass_clock_model_type;

typedef PACKED struct PACKED_POST
{
  uint8 gloEn;
  uint8 gloP1;
  uint8 gloP2;
  boolean gloM_incl;
  uint8 gloM;
  uint32 gloX;
  uint32 gloXdot;
  uint8 gloXdotdot;
  uint32 gloY;
  uint32 gloYdot;
  uint8 gloYdotdot;
  uint32 gloZ;
  uint32 gloZdot;
  uint8 gloZdotdot;
}rrc_glonass_ECEF_type;

typedef enum
{
  NAV_CLK_MODEL,
  CNAV_CLK_MODEL,
  GLONASS_CLK_MODEL,
  SBAS_CLK_MODEL
}rrc_ganss_clk_model_e_type;

typedef PACKED struct PACKED_POST
{
  uint8 utc_model_type; /* Of type rrc_ganss_clk_model_e_type */
  PACKED union PACKED_POST
  {
    PACKED struct PACKED_POST
    {
      boolean nav_clock_incl;
      rrc_nav_clock_model_type nav_clk_model;
    }nav;

    PACKED struct PACKED_POST
    {
      boolean cnavClockModel_inc;
      rrc_cnav_Clock_Model_type cnav_clk_model;
    }cnav;

    PACKED struct PACKED_POST
    {
      boolean glonass_Clock_incl;
      rrc_glonass_clock_model_type glonass_clk_model;
    }glo;

  }u;

}rrc_GANSS_clock_models_type;

typedef PACKED struct PACKED_POST
{
  boolean glonass_ECEF_incl;
  rrc_glonass_ECEF_type glonass_ECEF;
}rrc_GANSS_orbit_models_type;

typedef PACKED struct PACKED_POST
{
  uint8 sat_id;    /*0..63*/
  uint8 sv_health; /*BitString(5)*/
  uint16 iod;      /*BitString(10) */

  rrc_GANSS_clock_models_type ganss_clk_model;
  rrc_GANSS_orbit_models_type ganss_orbit_model;

}rrc_sat_info_nav_list_type;

/*typedef PACKED struct
{
  uint8 Signal_Id;
  float Status_Health;
  uint8 sat_cnt;
  rrc_signal_InformationList_type signal_info[MAX_GANSS_SAT];
}rrc_dganss_Info_List_type;*/

typedef PACKED struct PACKED_POST
{
  boolean non_broadcast_inc;
  boolean non_broadcast_ind;
  uint8 sat_cnt;  /*0..63*/
  rrc_sat_info_nav_list_type sat_info_list[MAX_GANSS_SAT];

}rrc_GANSS_add_navigation_models_type;

typedef PACKED struct PACKED_POST
{
  uint8 bad_ganss_satId;

  boolean bad_ganss_signalId_incl;
  uint8 bad_ganss_signalId;
}rrc_GANSS_RealTime_Integrity_type;

typedef PACKED struct PACKED_POST
{
  boolean ganss_sig_Id_incl;
  uint8 ganss_sig_Id;
  uint8 sat_cnt;
  rrc_gps_sat_info_struct_type sat_info[MAX_GANSS_SAT];

}rrc_GANSS_Ref_meas_Info_type;

typedef PACKED struct PACKED_POST
{
  uint16 gloAlmNA;
  uint8 gloAlmnA;
  uint8 gloAlmHA;
  uint32 gloAlmLambdaA;
  uint32 gloAlmTlambdaA;
  uint32 gloAlmDeltaIA;
  uint32 gloAkmDeltaTA;
  uint8 gloAlmDeltaTdotA;
  uint16 gloAlmEpsilonA;
  uint16 gloAlmOmegaA;
  uint16 gloAlmTauA;
  uint8 gloAlmCA;

  boolean gloAlmMA_incl;
  uint8 gloAlmMA;
}rrc_alm_glonass_Almanac_Set_type;

typedef PACKED struct PACKED_POST
{
  boolean wk_number_incl;
  uint8 wk_number;

  boolean almanac_set_incl;
  uint8 sat_cnt;
  rrc_alm_glonass_Almanac_Set_type almanac_set[MAX_GANSS_SAT];

}rrc_GANSS_Almanac_type;

typedef PACKED struct PACKED_POST
{
  uint16 utcA0;
  uint16 utcA1;
  uint8 utcA2;
  uint8 utcDeltaTls;
  uint16 utcTot;
  uint16 utcWNot;
  uint8 utcWNlsf;
  uint8 utcDN;
  uint8 utcDeltaTlsf;
}rrc_utcModel1_struct_type;

typedef PACKED struct PACKED_POST
{
  uint16 nA;
  uint32 tauC;

  boolean deltaUT1_incl;
  uint16 b1; /* if deltaUT1_incl = true*/
  uint16 b2; /* if deltaUT1_incl = true*/

  boolean kp_incl;
  uint8 kp;
}rrc_utcModel2_struct_type;

typedef PACKED struct PACKED_POST
{
  uint32 utcA1wnt;
  uint32 utcA0wnt;
  uint8 utcTot;
  uint8 utcWNt;
  uint8 utcDeltaTls;
  uint8 utcWNlsf;
  uint8 utcDN;
  uint8 utcDeltaTlsf;
  uint8 utcStandardID;
}rrc_utcModel3_struct_type;

typedef enum
{
  UTC_MODEL1,
  UTC_MODEL2,
  UTC_MODEL3
}rrc_utc_model_enum_type;

typedef PACKED struct PACKED_POST
{
  uint8 utc_model_type; /* Of type rrc_utc_model_enum_type */
  PACKED union PACKED_POST
  {
    rrc_utcModel1_struct_type utc_model_1;
    rrc_utcModel2_struct_type utc_model_2;
    /* RRC Transition Structure to be defined later */
    rrc_utcModel3_struct_type utc_model_3;
    /* SIB Structure to be defined later */
  }u;

}rrc_GANSS_Add_UTC_Models_type;

typedef PACKED struct PACKED_POST
{
  uint8 sv_id;
  uint8 signals_Available;
}rrc_Auxiliary_ganssID1_type;

typedef PACKED struct PACKED_POST
{
  uint8 sv_id;
  uint8 signals_Available;
  int channel_Number;
}rrc_Auxiliary_ganssID3_type;

typedef enum
{
  GANSS_ID1,
  GANSS_ID3
}rrc_Auxiliary_enum_type;


typedef PACKED struct PACKED_POST
{
  uint8 Auxiliary_type; /* Of type rrc_Auxiliary_enum_type */
  uint8 sat_cnt;
  PACKED union PACKED_POST
  {
    rrc_Auxiliary_ganssID1_type ganssID1[MAX_GANSS_SAT];
    /* RRC Transition Structure to be defined later */
    rrc_Auxiliary_ganssID3_type ganssID3[MAX_GANSS_SAT];
    /* SIB Structure to be defined later */
  }u;

}rrc_GANSS_Auxiliary_Info_type;

typedef PACKED struct PACKED_POST
{
  rrc_status_health_e_type status_health;

  boolean sig_id_incl;
  uint8 sig_id;

  boolean dganss_sig_info_list_incl;
  uint8 sat_cnt;
  rrc_dgps_correction_sat_Info_type dganss_sig_info_list[MAX_GANSS_SAT];

}rrc_dganss_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 dganss_ref_time;
  uint8 list_cnt;
  rrc_dganss_info_struct_type dganss_info[MAX_SGN_TYPE];

}rrc_dganss_correction_struct_type;


typedef PACKED struct PACKED_POST
{
  boolean ganss_id_incl;
  rrc_ganss_id_e_type ganssId;

  boolean time_mode_list_incl;
  uint8 list_cnt;
  rrc_ganss_time_mode_list_struct_type time_mode_list[MAX_GANSS];

  boolean DGANSS_corrections_incl;
  rrc_dganss_correction_struct_type DGANSS_corrections;

  boolean add_navigation_models_incl;
  rrc_GANSS_add_navigation_models_type add_navigation_models;

  boolean RealTime_Integrity_incl;
  uint8 sat_cnt;
  rrc_GANSS_RealTime_Integrity_type RealTime_Integrity[MAX_GANSS_SAT];

  boolean ref_meas_info_incl;
  rrc_GANSS_Ref_meas_Info_type ref_meas_info;

  boolean GANSS_Almanac_incl;
  rrc_GANSS_Almanac_type GANSS_Almanac;

  boolean add_utc_models_incl;
  rrc_GANSS_Add_UTC_Models_type add_utc_models;

  boolean Auxiliary_Info_incl;
  rrc_GANSS_Auxiliary_Info_type Auxiliary_Info;


}rrc_ganss_data_list_struct_type;
/* GANSS Assistance Data in MCM */


typedef PACKED struct PACKED_POST
{
  uint8 data_id;
  rrc_gps_iono_struct_type alpha_beta_param;

}rrc_ganss_addlono_model_struct_type;

typedef PACKED struct PACKED_POST
{
  /* GANSS Reference Time */
  boolean ganss_ref_time_incl;
  rrc_ganss_reference_time_struct_type ganss_ref_time;

  /* GPS DGPS Corrections. NOT Supported for now. Structure to be added later */

  /* GANSS Reference Position */
  boolean ganss_ref_pos_incl;
  rrc_gps_reference_loc_struct_type ganss_ref_pos;

  boolean ganss_addlono_model_incl;
  rrc_ganss_addlono_model_struct_type ganss_addlono_model;

  /* GANSS Data List */
  boolean ganss_data_list_incl;
  uint8 list_cnt;
  rrc_ganss_data_list_struct_type ganss_data_list[MAX_GANSS];

}rrc_ganss_assis_data_struct_type;


typedef enum
{
  GANSS_POS_GPS = 0,
  GANSS_POS_GALILEO,
  GANSS_POS_SBAS,
  GANSS_POS_MODERN_GPS,
  GANSS_POS_QZSS,
  GANSS_POS_GLONASS,
  GANSS_POS_NONE
} rrc_ganss_pos_method_e_type;


/* GPS Assistance Data in MCM */

typedef PACKED struct PACKED_POST
{
  /* GPS Reference Time */
  boolean gps_ref_time_incl;
  rrc_gps_reference_time_struct_type gps_ref_time;

  /* GPS Reference Location */
  boolean gps_ref_loc_incl;
  rrc_gps_reference_loc_struct_type gps_ref_loc;

  /* GPS DGPS Corrections. NOT Supported for now. Structure to be added later */

  /* GPS Navigation Model of Satellites Info */
  boolean gps_navigation_model_incl;
  rrc_gps_navigation_model_struct_type gps_navigation_model;

  /* GPS UTC Model */
  boolean gps_utc_model_incl;
  rrc_gps_utc_struct_type gps_utc_model;

  /* GPS Almanac. Reduced precision subset of ephemeris & clock correction params. */
  boolean gps_almanac_incl;
  /* Structure to be added as everything is a almanac */
  rrc_gps_almanac_struct_type gps_almanc;

  /* GPS Ionospheric Model */
  boolean gps_iono_incl;
  rrc_gps_iono_struct_type gps_iono;

  /* GPS Acquisition Assistance*/
  boolean gps_acq_assistance_incl;
  rrc_gps_acq_assistance_struct_type gps_acq_assistance;

  /* GPS Real Time Integrity i.e. Real Time status of the GPS constellation */
  boolean gps_real_time_integrity_incl;
  rrc_gps_rti_struct_type gps_rti;

  /* Reference Cell Info */
  boolean reference_cell_info_incl;
  /* Scrambling Code of the Reference Cell */
  uint16 psc;

  boolean gps_dgps_correction_incl;
  rrc_gps_dgps_correction_struct_type gps_dgps_correction;

  boolean ref_time_uncertainty_incl;
  rrc_gps_ref_time_Uncertainty_struct_type ref_time_uncert;

  boolean ganss_ref_time_incl;
  rrc_ganss_assis_data_struct_type ganss_assist_data;

  boolean ganss_nav_incl;
  rrc_GANSS_add_navigation_models_type ganss_nav;

  boolean ganss_almanac_incl;
  rrc_GANSS_Almanac_type ganss_almanac;

  boolean ganss_utc_model_incl;
  rrc_GANSS_Add_UTC_Models_type ganss_utc_model;

  boolean ganss_auxiliary_incl;
  rrc_GANSS_Auxiliary_Info_type ganss_auxiliary;
} rrc_assistance_data_struct_type;

/* Reporting Quantity */

typedef PACKED struct PACKED_POST
{
  /* Method Type */
  uint8 method_type; /* rrc_positioning_method_type_enum_type */

  /* Method */
  uint8 method; /* rrc_positioning_method_enum_type */

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

  uint8 environ_ch; /* rrc_environment_characterisation_enum_type */

  boolean r8_mcm;

  boolean vert_accuracy_incl;
  uint32 vertical_accuracy;

  boolean velocity_Req_inc;
  uint8 velocity_Requested;

  /*bit 0: GPS
  bit 1: Galileo
  bit 2: SBAS (WAAS, EGNOS,
           MSAS, GAGAN)
  bit 3: Modernized GPS (L1C, 
           L2C, L5)
  bit 4: QZSS
  bit 5: GLONASS
  bit 6-15: reserved for future GNSSes
  */
  boolean ganss_pos_method_incl;
  uint16 ganss_pos_method;

  /*Bit 0 is for Galileo.
  Bit 1 is for Modernized GPS;
  Bit 2 is for QZSS;
  Bit 3 is for GLONASS;
  Other bits are reserved.
  */
  boolean ganss_timing_cell_wanted_incl;
  uint16 ganss_timing_cell_wanted;

  boolean ganss_carrier_phase_meas_req_incl;
  uint16 ganss_carrier_phase_meas_req;

  boolean ganss_multi_freq_meas_req_incl;
  uint16 ganss_multi_freq_meas_req;

} rrc_rpt_quan_struct_type;

/* Periodic Reporting Structure */

typedef PACKED struct PACKED_POST
{
  uint8 rpt_amount;
    /* Amount of reporting (1,2,4,8,16,32,64, Infinity */
    /* Measurement is "released" after the indicated amount (number) of */
    /* report(s) from the UE itself. 7 means "infinity". As per LSM req, it would do conversion on its own */
  uint8  rpt_interval;
    /* Reporting Interval  0-8. As per LSM req, it would do conversion on its own */
} rrc_periodic_rpt_crit_struct_type;

/* GPS Reporting Criteria */

typedef PACKED struct PACKED_POST
{
  uint8 rpt_mode; /* rrc_report_crit_enum_type */

  PACKED union PACKED_POST  
  {
    /* TBD Later: Event Triggered Measurement reporting criteria */
    /* Periodical Measurement reporting criteria as in 10.3.7.53 */
    rrc_periodic_rpt_crit_struct_type periodic_crit;
  } u;
} rrc_reporting_criteria_struct_type;

/* A-GPS  Meas Control Structure */

typedef PACKED struct PACKED_POST
{
  /* Measurment identity */
  uint8       meas_id;

  /* Meas Command, Start/Modify/Delete */
  uint8 meas_cmd; /* rrc_meas_cmd_enum_type */

  /* True Indicates that RRC is expecting a confirmation message from LSM  */
  boolean     config_conf_ind;

  /* LSM should use this ack number while sending response to RRC in RRC_MEAS_CTRL_CNF */
  uint8       config_ack_num;

  /* Reporting Quantity. Always come with SETUP. Optional with Modify: Then LSM should use
     previous one */
  boolean rpt_quan_incl;

  rrc_rpt_quan_struct_type rpt_quan;


  /* Reporting Criteria. Always come with SETUP. Optional with Modify: Then LSM should use
     previous one */
  rrc_reporting_criteria_struct_type rpt_crit;

  /* GPS Assistance Data Included */
  boolean gps_assistance_data_incl;

  /* GPS Assistance Data */
  rrc_assistance_data_struct_type gps_assistance_data;

  /* Transfer Mode always specifed for a SETUP cmd.
  May not be specifed for a Modify cmd.
  If not specified, then LSM should use the
  previous one */
  boolean transfer_mode_incl;

  /* Report Transfer Mode. This value is always set by LSM while
   sending Measurement Report to RRC */
  uint8 tranfer_mode; /* rrc_report_transfer_mode_enum_type */

  /* Valid for SETUP cmd only. If set, then only measurement should be done. Else wait for a
     RESUME cmd from RRC */
  boolean meas_start_incl;
  /* GANSS Assistance Data Included */
  boolean ganss_assistance_data_incl;

  /* GANSS Assistance Data */
  rrc_ganss_assis_data_struct_type ganss_assistance_data;

} rrc_meas_ctrl_struct_type;


/* RRC_LSM_MEAS_REQ structure */

typedef PACKED struct PACKED_POST
{
  uint8 params_type; /* Of type rrc_meas_params_enum_type */

  PACKED union PACKED_POST
  {
    rrc_meas_ctrl_struct_type meas_ctrl_params;
    rrc_assistance_data_struct_type assistance_data_params;
    /* RRC Transition Structure to be defined later */
    rrc_state_trans_struct_type trans_params;
    /* SIB Structure to be defined later */
  } u;
} rrc_meas_params_struct_type;

typedef enum
{
  WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED = 0,
  WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED,
  WCDMA_CLASSMARK_UE_CAPABILITY_BOTH,
  WCDMA_CLASSMARK_UE_CAPABILITY_NONE
} network_assisted_gps_support_enum_type;


typedef enum
{
  GANSS_MODE_NW_BASED = 0,
  GANSS_MODE_UE_BASED,
  GANSS_MODE_BOTH,
  GANSS_MODE_NONE
} rrc_ganss_mode_e_type;


typedef PACKED struct PACKED_POST
{
  boolean ganss_id_incl;
  rrc_ganss_id_e_type ganss_id;

  rrc_ganss_mode_e_type ganss_mode;

  boolean ganss_sig_id_incl;
  uint8 ganss_sig_id;

  boolean ganss_timing_of_cell_frames;

  boolean ganss_carrier_phase_incl;
  boolean ganss_carrier_phase_meas;
}rrc_nw_assist_ganss_supported_list_s_type;

typedef struct
{
  boolean standalone_location_method_supported;
  boolean ue_based_otdoa_supported;
  network_assisted_gps_support_enum_type  network_assisted_gps_support;
  boolean gps_timing_of_cell_frames_supported;
  boolean ipdl_supported;
  boolean rx_tx_type2_supported;
  boolean up_meas_validity_cell_pch_and_ura_pch;
  boolean lcs_value_added_lcs_supported;

  boolean ganss_supported;

  boolean ganss_supported_list_incl;
  uint8 list_cnt;
  rrc_nw_assist_ganss_supported_list_s_type ganss_supported_list[MAX_GANSS];
  boolean ganssSupportIndication;
} tm_umts_cp_wcdma_ue_pos_capability_struct_type;


typedef enum /* for possible future expansion */
{
  RRC_CGPS_RRC_MSG_EVENT
} rrc_cgps_event_e_type;


typedef struct
{
  uint8*          p_data;
  uint32          length;
} rrc_cgps_msg_data_s_type;

typedef union /* for possible future expansion */
{
  rrc_cgps_msg_data_s_type    z_rrc_cgps_msg_data;
} rrc_cgps_event_data_union_type;

typedef struct
{
  rrc_cgps_event_e_type            e_event_type;
  rrc_cgps_event_data_union_type   u;
} rrc_cgps_event_data_s_type;

typedef boolean (* rrcgps_cgps_event_cb_type ) ( rrc_cgps_event_data_s_type* event_data_ptr );
typedef void (* rrcgps_cgps_ue_pos_capability_cb_type) ( tm_umts_cp_wcdma_ue_pos_capability_struct_type *out_capabilities_ptr );

typedef struct
{
  rrc_int_cell_id_type cell_id;
  uint16 psc;
  uint16 freq;
  rrc_plmn_identity_type plmn_id;
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
} rrcmeas_plmn_cell_info_struct_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum
{
  RRC_NEW_CELL_ID_AVAIL,
  RRC_NO_CELL_ID_AVAIL
}rrc_cell_id_status_type  ;


/*Call Back function will be called in response to RRC_GPS_CELL_ID_REQUEST*/
  typedef void 
RRC_GPS_CELL_ID_CNF_CB_FUNC_TYPE (
   rrcmeas_plmn_cell_info_struct_type  cell_id_info,
   rrc_cell_id_status_type  status 
);

/*===========================================================================

FUNCTION rrcmeas_get_cell_plmn_info

DESCRIPTION

 Returns the PLMN id, Cell id ,Psc, Freq of the current serving cell.
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

boolean rrcmeas_get_cell_plmn_info
(
  rrcmeas_plmn_cell_info_struct_type* plmn_cell_info_ptr
);
/*===========================================================================

FUNCTION rrc_lsm_meas_report_notification

DESCRIPTION

 Calls LSM fn which indicates if last report was received
 successfully by the network

DEPENDENCIES
 None

RETURN VALUE

 Void

SIDE EFFECTS

 None

===========================================================================*/
extern void rrc_lsm_meas_report_notification
(
  uint16 meas_id,
  boolean meas_rpt_txted
);

/* defined in rrcgpsmeas.c, for CGPS_UMTS_WCDMA to register the RRC event
   call back function */
boolean rrcgps_register_cgps_event_cb ( rrcgps_cgps_event_cb_type event_cb_fp );


/* defined in rrcgpsmeas.c, for CGPS_UMTS_CP_WCDMA to register the RRC UE
   capability call back function */
boolean rrcgps_register_cgps_ue_pos_capability_cb ( rrcgps_cgps_ue_pos_capability_cb_type ue_cap_cb_fp);

/*===========================================================================

FUNCTION  rrc_register_cell_id_cnf_cb

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
boolean rrc_register_cell_id_cnf_cb
(
  RRC_GPS_CELL_ID_CNF_CB_FUNC_TYPE  *cb_ptr
);

/*===========================================================================

FUNCTION  rrc_check_wcdma_in_oos

DESCRIPTION

  This function is called by GPS to check if W searching is in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If W search is in progress
  else
  FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_check_wcdma_in_oos
(
  void
);
/*===========================================================================

FUNCTION rrc_return_wcdma_rrc_version

DESCRIPTION
  This function returns the wcdma rrc version to gps.

DEPENDENCIES
  None.

RETURN VALUE
  RRC release version

SIDE EFFECTS
  None.

===========================================================================*/
uint8 rrc_return_wcdma_rrc_version(void);


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

#endif  /* RRCLSMIF_H */


