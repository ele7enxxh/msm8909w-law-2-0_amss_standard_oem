/* stats_event.h
 *
 * Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __STATS_EVENT_H__
#define __STATS_EVENT_H__
#ifdef FEATURE_GYRO
#include "sns_sam_gyroint_v01.h"
#endif

#include "mct_event_stats.h"
#define STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE (32)

typedef struct _gyro_samples{
  int32_t value[3];
  uint64_t timestamp;
} gyro_samples_t;


/** mct_event_gyro_data_t
 *    @frame_id:      the id of frame
 *    @ready:         indicate whether the data is ready
 *    @sample_len:    indicate the number of samples in @sample
 *    @sample:        the array of sample
 *    @sof:           the timestamp of SOF
 *    @frame_time:    the duration of frame
 *    @exposure_time: the expoure value
 *
 *  This structure is used to store gyro data
 *
 **/
typedef struct _mct_event_gyro_data {
  uint32_t frame_id;
  int      ready;
  uint32_t sample_len;
#ifdef FEATURE_GYRO
//  sns_sam_gyroint_sample_t_v01 sample[SNS_SAM_GYROINT_MAX_BUFSIZE_V01 * 2];
gyro_samples_t sample[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
#endif
  uint64_t sof;
  uint64_t frame_time;
  float    exposure_time;
} mct_event_gyro_data_t;

/** mct_event_gyro_stats_t
 *    @q16_angle:  the angle for x, y, z
 *    @ts:         the timestamp
 *
 *  This structure is used to store gyro data
 *
 **/
typedef struct _mct_event_gyro_stats {
  int      q16_angle[3];
  uint64_t ts;
  mct_event_gyro_data_t is_gyro_data;
} mct_event_gyro_stats_t;

/** _mct_event_gravity_vector_update
 *  @gravity:  gravity vector (x/y/z) in m/s/s
 *  @lin_accel: linear acceleration (x/y/z) in m/s/s
 *  @accuracy: sensor accuracy (unreliable/low/medium/high)
 *
 *  This structure is used to store and trasnmit gravity
 *  vector received from the motion sensor.
 *
 **/
typedef struct _mct_event_gravity_vector_update {
  float gravity[3];
  float lin_accel[3];
  uint8_t accuracy;
} mct_event_gravity_vector_update_t;

/** _mct_event_gravity_vector_enable
 *  @enable: boolean flag - true if enable false if disable.
 *  @report_period: how often to get gravity data, units of
 *    seconds, Q16
 *  @sample_rate_valid: true if sampling rate is passed.
 *  @sample_rate: sample rate in Hz, Q16. If less than report
 *    period, it is set to report rate.
 *
 *  This structure is used to enable/disable gravity vector.
 *
 **/
typedef struct _mct_event_gravity_vector_enable {
  boolean enable;
  uint32_t report_period;
  uint8_t sample_rate_valid;
  uint32_t sample_rate;
} mct_event_gravity_vector_enable_t;

typedef struct _mct_event_stats_ext_t {
  pthread_mutex_t*        stats_mutex;
  int32_t                 ref_count;
  mct_event_stats_isp_t*  stats_data;
} mct_event_stats_ext_t;


#endif /* __STATS_EVENT_H__ */
