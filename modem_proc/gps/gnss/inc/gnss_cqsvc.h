#ifndef GNSS_CQSVC_H
#define GNSS_CQSVC_H
/*============================================================================
  @file gnss_cqsvc.h
        ( GNSS Common QMI Service )
        
  @brief
   
    This module provides a service agnostic interface to the two qmi-services
    that Location Engine Provides. The two services are QMI-PDS and QMI-LOC.

    The definitions in this module are parallel to the ones that are defined
    in the QMI-Services. This is done so that the engine is decoupled from the
    APIs.    

    - For Outgoing Messages (ie the messages going to the QMI-Clients)
        This module is just going to route the message to the registered 
        service. Only one service can be active at any time. This module will 
        not do any error-checking of the message, and the relay will happen in
        the callers task context.
    
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/gnss_cqsvc.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
2012-02-09  spn  Initial revision.


=============================================================================*/

/*****************************************************************************
 * Include Files
 * *************************************************************************/
  /* Include files */
#include <stdint.h>

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/


/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

typedef struct {

  uint16_t gps_week;
  /**<   GPS week (full GPS week).  */

  uint32_t gps_time_of_week_ms;
  /**<   GPS time of week in milliseconds.  */
}cqsvc_gps_tow_struct_type;  /* Type */


typedef enum {
  eCQSVC_TIME_SOURCE_ESTIMATE_ENUM_TYPE_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eCQSVC_TIME_SOURCE_ESTIMATE_INVALID = 0, 
  eCQSVC_TIME_SOURCE_ESTIMATE_NETWORK_TIME_TRANSFER = 1, 
  eCQSVC_TIME_SOURCE_ESTIMATE_NETWORK_TIME_TAGGING = 2, 
  eCQSVC_TIME_SOURCE_ESTIMATE_EXTERNAL_INPUT = 3, 
  eCQSVC_TIME_SOURCE_ESTIMATE_TOW_DECODE = 4, 
  eCQSVC_TIME_SOURCE_ESTIMATE_TOW_CONFIRMED = 5, 
  eCQSVC_TIME_SOURCE_ESTIMATE_TOW_AND_WEEK_CONFIRMED = 6, 
  eCQSVC_TIME_SOURCE_ESTIMATE_TIME_ALIGNMENT = 7, 
  eCQSVC_TIME_SOURCE_ESTIMATE_NAV_SOLUTION = 8, 
  eCQSVC_TIME_SOURCE_ESTIMATE_SOLVE_FOR_TIME = 9, 
  eCQSVC_TIME_SOURCE_ESTIMATE_ENUM_TYPE_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cqsvc_time_source_estimate_enum_type;


typedef enum {
  eCQSVC_SATELLITE_INFO_SV_ENUM_TYPE_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eCQSVC_SVN_SYSTEM_INVALID = 0x00,
  eCQSVC_SVN_SYSTEM_GPS = 0x01,
  eCQSVC_SVN_SYSTEM_GALILEO = 0x02,
  eCQSVC_SVN_SYSTEM_SBAS = 0x03,
  eCQSVC_SVN_SYSTEM_COMPASS = 0x04,
  eCQSVC_SVN_SYSTEM_GLONASS = 0x05,
  eCQSVC_SVN_SYSTEM_BDS = 0x06,
  eCQSVC_SVN_SYSTEM_QZSS = 0x07,
  eCQSVC_SATELLITE_INFO_SV_ENUM_TYPE_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cqsvc_satellite_info_sv_enum_type;


typedef enum {
  eCQSVC_ENGINE_ERROR_RESET_ENUM_TYPE_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_POSITION_ENGINE = 0, /**<  Reset caused by Position Engine  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_BACKGROUND_SCAN = 1, /**<  Reset caused by GNSS Background Scan  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_CLOCK_INCONSISTENCY = 2, /**<  Reset caused by injected clock inconsistency  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_GPS_SUBFRAME_ALIGNMENT = 3, /**<  Reset caused by GPS Subframe misalignment  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_DECODED_TIME_INCONSISTENCY = 4, /**<  Reset caused by decoded time inconsistency  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_MEASUREMENT_INCONSISTENCY = 5, /**<  Reset caused by code consistency error  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_GNSS_INTMS_CONSISTENCY = 6, /**<  Soft Reset caused by INTMS(integer millisecond) error  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_GNSS_RF_FAILURE = 7, /**<  Soft reset caused by RF Failure  */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_CC_SOFT_ERROR_RECOVERY = 8, /**< Channel Controller soft error recovery due to 
                                                                      (1) Receiver CPU starvation
                                                                      (2) DMA Error
                                                                      (3) H/w register programming     */
  eCQSVC_ENGINE_ERROR_RECOVERY_RESET_CC_HARD_ERROR_RECOVERY = 9, /* Channel controller complete error recovery */
								 /* triggered after soft error recovery fails */
  eCQSVC_ENGINE_ERROR_RESET_ENUM_TYPE_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cqsvc_error_reset_enum_type;

typedef uint32_t cqsvc_error_reset_delete_mask_type;
#define CQSVC_CLOCK_DELETED_MASK ((cqsvc_error_reset_delete_mask_type)0x00000001) /**<  Clock information is deleted  */
#define CQSVC_POSITION_DELETED_MASK ((cqsvc_error_reset_delete_mask_type)0x00000002) /**<  Position Information is deleted  */
#define CQSVC_SV_DIRECTIONS_DELETED_MASK ((cqsvc_error_reset_delete_mask_type)0x00000004) /**<  SV Direction Information is deleted  */
#define CQSVC_SV_STEERING_DELETED_MASK ((cqsvc_error_reset_delete_mask_type)0x00000008) /**<  SV Steering information is deleted  */

typedef struct {

  cqsvc_satellite_info_sv_enum_type svn_system;
  uint8_t svn_id;
  /**<   SV ID of the satellite. Range: \n
       - GPS -- 1 to 32 \n
       - SBAS -- 33 to 64 \n
       - GLONASS -- 65 to 96 \n 
       - Quasi-Zenith Satellite System (QZSS) -- 193 to 197 */
  uint16_t cno_drift;
  /**<   CN0 drift (in dBHz * 10.0). */
  uint16_t sub_msec_diff;
  /**<   Sub-millisecond difference in microseconds between steering and 
       observed measurement. */
  int16_t doppler_diff;
  /**<   Doppler difference in Hz between steering and observed measurement. */
}cqsvc_error_reset_background_scan_params_struct_type;  /* Type */

typedef struct {

  cqsvc_satellite_info_sv_enum_type svn_system;
  cqsvc_time_source_estimate_enum_type cur_time_source;
  cqsvc_time_source_estimate_enum_type injected_time_source;
  uint32_t time_diff_msecs;
  /**<   Clock difference in milliseconds between the system clock and 
           injected time. */
  uint32_t time_unc;
  /**<   Time uncertainty in microseconds. */
}cqsvc_error_reset_clock_inconsistency_struct_type;  /* Type */

typedef enum {
  eCQSVC_RF_LINEARITY_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eCQSVC_RF_LINEARITY_LO_V01 = 0, /**<  GPS data demodulation is complete. GNSS votes for Low 
       power mode.  */
  eCQSVC_RF_LINEARITY_HI_V01 = 1, /**<   GPS data demodulation is in progress. Need to stay in 
        High power mode  */
  eCQSVC_RF_LINEARITY_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cqsvc_rf_linearity_enum_type;

typedef struct {

  uint8_t svn_id;
  cqsvc_rf_linearity_enum_type rf_linearity;
  uint16_t ultra_confidence;
  /**<   Number of cycle slips observed. */
  uint16_t parity_errors;
  /**<   Parity errors observed on the GPS subframe. */
  uint32_t word;
  /**<   GPS subframe word with timing error. */
}cqsvc_error_reset_subframe_alignment_struct_type;  /* Type */

typedef struct {
  cqsvc_satellite_info_sv_enum_type svn_system;
  uint8_t svn_id;
  /**<   SV ID of the satellite. Range: \n
       - GPS -- 1 to 32 \n
       - SBAS -- 33 to 64 \n
       - GLONASS -- 65 to 96 \n 
       - QZSS -- 193 to 197 */
  cqsvc_gps_tow_struct_type gps_time;
  /**<   Decoded time by the satellite. */
  uint16_t gnss_num_of_days;
  /**<   GLONASS number of days if the SVN system is GLONASS. */
  uint32_t gnss_msecs;
  /**<   GLONASS milliseconds if the SVN system is GLONASS. */
  int32_t clock_diff_msecs;
  /**<   Clock difference between the decoded time and current time in the 
       receiver. Units: milliseconds. */
}cqsvc_error_reset_decoded_time_err_struct_type;  /* Type */

typedef struct {
  cqsvc_satellite_info_sv_enum_type svn_system;
  cqsvc_gps_tow_struct_type gps_time;
  /**<   Current receiver time. */
  uint8_t total_consistency_failures;
  /**<   Total number of measurement inconsistencies observed. */
  uint16_t duration;
  /**<   Duration for which the consistency failures are observed. */
}cqsvc_error_reset_code_carrier_err_struct_type;  /* Type */

typedef struct {
  uint32_t old_bp_ampl;
  /**<   Observed GPS signal strength when the signal was good. */
  uint32_t new_bp_ampl;
  /**<   Observed GPS signal strength when a reset was forced. */
}cqsvc_error_reset_rf_err_struct_type;  /* Type */

typedef struct {
  /* Reason for the reset.*/
  cqsvc_error_reset_enum_type reset_type;

  /* Mask identifying the deleted assistance data. */
  cqsvc_error_reset_delete_mask_type assist_delete_mask;

  /*  Current System Time */
  uint8_t gps_time_valid;  /**< Must be set to true if gps_time is being passed */
  cqsvc_gps_tow_struct_type gps_time;

  /*  Background Scan Parameters */
  uint8_t background_scan_params_valid;  /**< Must be set to true if background_scan_params is being passed */
  cqsvc_error_reset_background_scan_params_struct_type background_scan_params;

  /*  Clock Inconsistency Parameters */
  uint8_t clock_inconsistency_params_valid;  /**< Must be set to true if clock_inconsistency_params is being passed */
  cqsvc_error_reset_clock_inconsistency_struct_type clock_inconsistency_params;

  /*  Subframe Alignment Parameters */
  uint8_t subframe_alignment_params_valid;  /**< Must be set to true if subframe_alignment_params is being passed */
  cqsvc_error_reset_subframe_alignment_struct_type subframe_alignment_params;

  /*  Time Inconsistency Parameters */
  uint8_t time_inconsistency_params_valid;  /**< Must be set to true if time_inconsistency_params is being passed */
  cqsvc_error_reset_decoded_time_err_struct_type time_inconsistency_params;

  /*  Measurement Inconsistency Parameters */
  uint8_t code_carrier_params_valid;  /**< Must be set to true if code_carrier_params is being passed */
  cqsvc_error_reset_code_carrier_err_struct_type code_carrier_params;

  /*  RF Parameters  */
  uint8_t rf_params_valid;  /**< Must be set to true if rf_params is being passed */
  cqsvc_error_reset_rf_err_struct_type rf_params;
}cqsvc_engine_err_recovery_rpt_ind_msg;  /* Message */


/*****************************************************************************
 * Function Declarations and Documentation
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the location engine, to send an error
  recovery report to the registered clients.

@warning
  - The information will be relayed as is. The information will be assumed to 
    be correct, minimal or no error checking will be performed
  - The information will be relayed in the callers task-context to the 
    QMI-Framework.

@retval  TRUE  - Successful relay of the message to the QMI-Framework
@retval  FALSE - Message was not relayed to the QMI-Framework
*//* ------------------------------------------------------------------------*/
boolean gnss_CQmiSvcSendEngineErrorRecoveryReport
( 
  cqsvc_engine_err_recovery_rpt_ind_msg *pz_ErrReport 
);

#endif /* #ifndef GNSS_CQSVC_H */

