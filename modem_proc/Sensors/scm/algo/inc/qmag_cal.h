#ifndef QMAG_CAL_H
#define QMAG_CAL_H

/*============================================================================
@file qmag_cal.h

  Qualcomm Magnetometer calibration algorithm header file

Copyright (c) 2013, 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/scm/algo/inc/qmag_cal.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
EDIT HISTORY

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 

when        who  what, where, why
----------  ---  -----------------------------------------------------------
2016-02-10  jhh  Added tuning parameter support
2015-03-26  ps   Updated license header
2013-06-13  ks   initial version

============================================================================*/

#include <stdint.h>
#include "sns_buffer.h"
#include "circular_buffer.h"
#include "fixed_point.h"
#include "matrix_float.h"
#include "sns_em.h"

//#define GYRO
#define QMAG_CAL_COL (3)

#define QMAG_CAL_CALIBRATION_CYCLE_SEC (0.5f)
#define QMAG_CAL_MOTION_WINDOW_SEC (1.25f*QMAG_CAL_CALIBRATION_CYCLE_SEC)

#define QMAG_CAL_HI_CAL_WIN_SIZE_SECOND (2.5f)
#define QMAG_CAL_HI_CAL_WIN_SIZE_USEC (QMAG_CAL_HI_CAL_WIN_SIZE_SECOND*1000000.0f)
#define QMAG_CAL_DECIM_WINDOW_SEC QMAG_CAL_HI_CAL_WIN_SIZE_SECOND

#if defined(OFFTARGET_UNIT_TESTING) || defined(SNS_PCSIM)
#define QMAG_CAL_HI_CAL_WIN_SIZE_TICKS ((uint32_t)(QMAG_CAL_HI_CAL_WIN_SIZE_SECOND/(30.51/1000000.0)))
#else
#define QMAG_CAL_HI_CAL_WIN_SIZE_TICKS (sns_em_convert_usec_to_dspstick((uint32_t)(QMAG_CAL_HI_CAL_WIN_SIZE_USEC)))
#endif

#define QMAG_CAL_MIN_MAG_SAMPLERATE (10.0f)
#define QMAG_CAL_MIN_MAG_SAMPLERATE_Q16 FX_FLTTOFIX_Q16(QMAG_CAL_MIN_MAG_SAMPLERATE)
#define QMAG_CAL_MAX_MAG_SAMPLERATE (50.0f)
#define QMAG_CAL_MAX_MAG_SAMPLERATE_Q16 FX_FLTTOFIX_Q16(QMAG_CAL_MAX_MAG_SAMPLERATE)
#define QMAG_CAL_NUM_HI_SAMPLE_MULTIPLIER_AT_SR_ABOVE_10 (0.4f)
#define QMAG_CAL_MIN_MAG_SAMPLES_AUTONOMOUS_10HZ (18.0f)
#define QMAG_CAL_MIN_NEW_SAMPLES_10HZ (ceil(QMAG_CAL_MIN_MAG_SAMPLES_AUTONOMOUS_10HZ/3.0))
#define QMAG_CAL_MAG_NOISE_STD_FOR_VARIANCE (0.03f)

#define QMAG_CAL_SPAN_STATS_LENGTH (7) // XYZ and 4 bisectors
#define QMAG_CAL_BISECTOR_SPAN_STATS_OFFSET (QMAG_CAL_COL) // which is 3

#define QMAG_CAL_MAG_VARIANCE_WINDOW_SEC (0.25f)
#define QMAG_CAL_ACCEL_VARIANCE_WINDOW_SEC (0.25f)
#define QMAG_CAL_GYRO_VARIANCE_WINDOW_SEC (0.25f)
#define QMAG_CAL_MAG_VARIANCE_THRESH (0.0003f) // consider lowering for low-noise mag models
#define QMAG_CAL_ACCEL_VARIANCE_THRESH (0.3253f) // for 50 Hz
#define QMAG_CAL_GYRO_VARIANCE_THRESH (0.13f*0.13f) // (7.4 rad/s)^2
#define QMAG_CAL_MIN_GYRO_TURN_RATE_SQ (0.5*0.5) // (rad/s)^2, current gyro norm ^2


#define QMAG_CAL_HARD_RESET_THRESH_GAUSS_SQ  (2.0f*2.0f) //  NOT QX16

#define QMAG_CAL_CALIBRATION_HISTORY (10)
#define QMAG_CAL_RADIUS_HISTORY (10)

#define QMAG_CAL_MAX_INV_MATRIX_ROWS (4) // in fact, this is the only matrix inverted in the algo, in curve_fit_spheroid()
/*mag calibration accuracy*/
typedef enum
{
   QMAG_CAL_ACCURACY_FAILED = 0,
   QMAG_CAL_ACCURACY_LOW = 1,
   QMAG_CAL_ACCURACY_MEDIUM = 2,
   QMAG_CAL_ACCURACY_HIGH = 3,
   QMAG_CAL_ACCURACY_NUM = 4
} qmag_cal_accuracy_e;

#define QMAG_CAL_MIN_HI_GOOD_ACCURACY   QMAG_CAL_ACCURACY_LOW

typedef enum
{
   QMAG_CAL_INPUT_TYPE_MAG = 0,
   QMAG_CAL_INPUT_TYPE_ACCEL = 1,
   QMAG_CAL_INPUT_TYPE_GYRO = 2,
   QMAG_CAL_INPUT_TYPE_GYRO_QUATERNION = 3
} qmag_cal_input_type_e;

typedef struct
{
   float published_offs_HI[3];
   uint8_t published_accuracy_HI;
   uint32_t published_offset_time;
   uint8_t published_offset_valid;
} qmag_cal_persistent_state_s;

/*mag calibration tuning parameters*/
typedef struct
{
   //from reg
   int32_t qmag_cal_calibration_history;
   int32_t qmag_cal_radius_history;

   float qmag_cal_min_radius_gauss;
   float qmag_cal_max_radius_gauss;

   float min_rel_data_span_hi_accuracy_0;
   float min_rel_data_span_hi_accuracy_1;
   float min_rel_data_span_hi_accuracy_2;
   float min_rel_data_span_hi_accuracy_3;

   float min_accel_seg_variance_hi_0;
   float min_accel_seg_variance_hi_1;
   float min_accel_seg_variance_hi_2;
   float min_accel_seg_variance_hi_3;

   float qmag_cal_min_decimation_delta_sq;
   float qmag_cal_max_decimation_delta_sq;

   float qmag_cal_min_decimation_gyro_angle;
   
   float qmag_cal_default_3d_mag_field_intensity; //QMAG_CAL_DEFAULT_3D_MAG_FILED_INTENSITY?

   float qmag_cal_hi_cal_win_size_second;
   float qmag_cal_calibration_cycle_sec;
   float qmag_cal_motion_window_sec;

   float qmag_cal_min_mag_samples_autonomous_10hz;

   float qmag_cal_mag_variance_window_sec;
   float qmag_cal_accel_variance_window_sec;
   float qmag_cal_gyro_variance_window_sec;

   float qmag_cal_mag_variance_thresh;
   float qmag_cal_accel_variance_thresh;
   //float qmag_cal_gyro_variance_thresh;

   //internal
   float qmag_cal_default_decimation_delta_sq;
   float qmag_cal_hi_cal_win_dize_usec;
   float qmag_cal_decim_window_sec;
   float qmag_cal_hi_cal_win_size_ticks;

} qmag_cal_tuning_param_s;

/*mag calibration configuration*/
typedef struct
{
   uint32_t sample_rate_mag;          // mag sample rate, Hz, Q16
   uint32_t sample_rate_accel;        // accel sample rate, Hz, Q16
#ifdef GYRO
   uint32_t sample_rate_gyro;         // gyro sample rate, Hz, Q16. Set to zero if gyro unavilable
#endif
   qmag_cal_persistent_state_s persistent_state;

   int32_t publish_HI_delay;         // offset_HI_buff_count must be at least this before publishing HIGH
   bool publish_max_buffered_accuracy;

   qmag_cal_tuning_param_s qmag_cal_tuning_param;
} qmag_cal_config_s;

typedef struct
{
   int32_t sensor_data[QMAG_CAL_COL];
   uint32_t sensor_time;
} qmag_cal_sensor_data_s;

/*mag calibration input*/
typedef struct
{
   int32_t sensor_data[QMAG_CAL_COL];
   uint32_t sensor_time;
   qmag_cal_input_type_e input_type;
} qmag_cal_input_s;

/*mag calibration output*/
typedef struct
{
   bool offset_updated;
   int32_t offset_HI[QMAG_CAL_COL];
   qmag_cal_accuracy_e accuracy_HI;
   uint32_t offset_time; // that will be equal to input sensor time
} qmag_cal_output_s;

/*mag calibration  state*/
typedef struct
{
   qmag_cal_config_s config;           // MAG calibration configuration
   qmag_cal_persistent_state_s persistent_state;

#ifdef PREALLOCATE_MATRIX_INVERSION_BUFFER
   matrix_inversion_buffer_s matrix_inversion_buff;
#endif
   uint64_t decim_ts_delta_sum; // sum of mag samples time stamps being used in current decimation cycle

   double accel_raw_buf_sum[QMAG_CAL_COL];
   double accel_raw_buf_sum_sq[QMAG_CAL_COL];
   double mag_raw_buf_sum[QMAG_CAL_COL];
   double mag_raw_buf_sum_sq[QMAG_CAL_COL];
   /* double */ float accel_raw_variance;
   /* double */ float mag_raw_variance;
#ifdef GYRO
   double gyro_raw_buf_sum[QMAG_CAL_COL];
   double gyro_raw_buf_sum_sq[QMAG_CAL_COL];
   float gyro_raw_variance;
#endif

   float sample_rate_mag; // Hz
   float sample_rate_accel; // Hz
#ifdef GYRO
   float sample_rate_gyro; // Hz, zero if unavailbale
   float approx_gyro_sample_interval; // second
#endif

   float last_accel_raw[QMAG_CAL_COL]; // last received accel sample
#ifdef GYRO
   float last_gyro_raw[QMAG_CAL_COL]; // last received gyro sample
#endif
   uint32_t input_mag_sample_count_since_reset; // number of mag samples received since reset
   uint32_t input_mag_sample_count; // number of mag samples received since last decimated point
   uint32_t decim_num_average_samples;
   float mag_sum_between_decim_samples[QMAG_CAL_COL]; // sum of mag samples since last decimated point
   float decim_last_decimated_mag[QMAG_CAL_COL]; // last computed decimated mag
   float accel_sum_between_decim_samples[QMAG_CAL_COL]; // sum of accel samples since last decimated point
   float decim_last_decimated_accel[QMAG_CAL_COL]; // last computed decimated accel
#ifdef GYRO
   float gyro_delta_angle_sum_between_motion_samples; // just accumulator, no threshold for it
   float gyro_delta_angle_sum_between_decim_samples; // will be checked against threshold
   float decim_gyro_angle; // to be buffered
   float decim_gyro_angle_sum_on_cal_seg;
#endif
   uint32_t decim_last_decimated_ts; // last computed decimated tmag
   uint32_t num_new_decim_samples;
   float acc_var_sum;

   // spheroid fit
   uint32_t min_mag_samples_autonomous;
   uint32_t min_new_samples;

   float offset_HI[QMAG_CAL_COL];
   float radius_HI;
#ifdef COMPUTE_HI_COV_MAT
   float cov_mat_HI[QMAG_CAL_COL+1][QMAG_CAL_COL+1];
#endif
   float rel_rad_std_HI;
   float rel_rad_span_HI;

   float min_3d_span_HI;
   float max_3d_span_HI;
   float rel_data_span_min_HI;
   float rel_data_span_max_HI;
   float min_rel_data_span_HI[QMAG_CAL_ACCURACY_NUM];
   float max_rel_data_span_HI[QMAG_CAL_ACCURACY_NUM];
   float max_rel_rad_std_HI[QMAG_CAL_ACCURACY_NUM];
   float max_rel_rad_span_HI[QMAG_CAL_ACCURACY_NUM];
   float max_trace_offs_covmat_HI[QMAG_CAL_ACCURACY_NUM];
   float min_accel_seg_variance_HI[QMAG_CAL_ACCURACY_NUM];
#ifdef GYRO
   float min_gyro_angle_HI[QMAG_CAL_ACCURACY_NUM];
#endif
   float min_HI_R_deviation_from_median_R_rel[QMAG_CAL_ACCURACY_NUM];
   float max_HI_R_deviation_from_median_R_rel[QMAG_CAL_ACCURACY_NUM];

   // buffered raw sensor samples
   sns_buffer_type mag_raw_buf; // raw magnetometer samples in Gauss
   sns_buffer_type accel_raw_buf; // accel samples in m/s2
#ifdef GYRO
   sns_buffer_type gyro_raw_buf; // gyro samples dps and and delta angle rad
#endif
   // buffered motion sensor samples
   sns_buffer_type mag_motion_buf; // motion magnetometer samples in Gauss
   sns_buffer_type accel_motion_buf; // motion accel samples in m/s2
#ifdef GYRO
   sns_buffer_type gyro_delta_angle_motion_buf; // rad, N*1, from previous motion sample
#endif
   buffer_type ts_motion_buf; // motion magnetometer and accel common timestamps

   // buffered decimated samples
   sns_buffer_type mag_decim_buf; // decimated magnetometer samples in Gauss
   sns_buffer_type accel_decim_buf; // decimated accel samples in m/s2
#ifdef GYRO
   // cumulative would've been better but that would have to be double
   sns_buffer_type gyro_delta_angle_decim_buf; // N*1, sqrt(XYZ_sq_sum)between adjoining decim mag samples
#endif
#ifdef GYRO_QUAT
   double cumulative_gyro_wxyz_quaternion_at_gyro_sample_rate[4];
   sns_buffer_type cumulative_gyro_wxyz_quaternion_at_decim_time; // N*4, from quaternion algo or mag cal algo reset
#endif
   buffer_type ts_decim_buf; // decimated magnetometer and accel common timestamps
   float **HI_N_by_4_2D_buf;
   float **HI_N_by_1_2D_buf;
   float **HI_curve_fit_buffer_16_by_4; // will be chopped into four 4*4 temp buffers
#ifdef SI
   float offset_SI[QMAG_CAL_COL];
   float radius_SI;
   float cond_nr_SI;
   float rel_rad_std_SI;
   float rel_rad_span_SI;
   float **Smat_SI; // 3*3
   float **SI_N_by_10_meas_buf;
   float **SI_N_by_3_cal_XYZ_buf;
   float **SI_10_by_10_covmat_buf;
#endif
   // buffered output of hard iron calibration
   float radius_HI_buff[QMAG_CAL_RADIUS_HISTORY][1];
   float offset_HI_buff[QMAG_CAL_CALIBRATION_HISTORY][QMAG_CAL_COL];
   float accuracy_HI_buff[QMAG_CAL_CALIBRATION_HISTORY][1]; // floating point is ok because we never need to convert it to integer

   float min_offset_change_for_soft_reset[QMAG_CAL_ACCURACY_NUM];

   float last_good_radius;
   float min_decimation_delta_sq;

   qmag_cal_accuracy_e accuracy_HI;

   uint8_t offset_HI_buff_count;
   uint8_t radius_HI_buff_count;
   uint8_t accuracy_HI_buff_count;
   bool use_gyro;
   bool new_motion_sample; // flag indicating if new motion samples have been added to buffer

} qmag_cal_state_s;

/*=========================================================================
FUNCTION:  qmag_cal_scm_mem_req
=========================================================================*/
/*!
@brief
Provides the algorithm state memory requirement for the specified config

@param[i] config: pointer to the specified algorithm configuration

@return
algorithm state memory requirement in bytes if successful, 0 if error
*/
/*=======================================================================*/
int32_t qmag_cal_scm_mem_req(qmag_cal_config_s *config);

/*=========================================================================
FUNCTION:  qmag_cal_scm_state_reset
=========================================================================*/
/*!
@brief
Reset the algorithm state

@param[i] config: pointer to the specified algorithm configuration
@param[i] mem: pointer to the memory allocated for algorithm state

@return
pointer to algorithm state
*/
/*=======================================================================*/
qmag_cal_state_s* qmag_cal_scm_state_reset(
   qmag_cal_config_s *config,
   void *mem);

/*=========================================================================
FUNCTION:  qmag_cal_scm_update
=========================================================================*/
/*!
@brief
Execute the algorithm in the specified state using specified input
to generate the output at the specified location

@param[i] state: pointer to algorithm state
@param[i] input: pointer to algorithm input
@param[i] output: pointer to algorithm output

@return None
*/
/*=======================================================================*/
void qmag_cal_scm_update(
                         qmag_cal_state_s *state,
                         qmag_cal_input_s *input,
                         qmag_cal_output_s *output);

/*=========================================================================
FUNCTION:  qmag_cal_extract_data_from_cyclic_buffer
=========================================================================*/
/*!
@brief
Extracts data from cyclic buffer to a 2d array

@param[i] cyclic_buff: pointer to sns_buffer_type
@param[i] start_index: starting index of data to be extracted in cyclic buffer
@param[i] stop_index: ending index of data to be extracted in cyclic buffer
@param[o] matX: pointer to 2 array to which data is extracted to

@return boolean set to true if succesful
*/
/*=======================================================================*/
bool qmag_cal_extract_data_from_cyclic_buffer(sns_buffer_type *cyclic_buff,
                                              uint32_t start_index,
                                              uint32_t stop_index,
                                              float **matX );

uint32_t compute_circular_buffer_num_data_rows(uint32_t start_index, uint32_t stop_index, uint32_t buf_count);


#endif /* QMAG_CAL_H */
