/* depth_service.h
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef DEPTH_SERVICE_H_
#define DEPTH_SERVICE_H_
#include "mtype.h"
#include "mct_event_stats.h"
#include "mct_module.h"
#include "modules.h"

/* Enum definition */
#define MAX_DIST_MAP_H_NUM     16
#define MAX_DIST_MAP_V_NUM     12
#define MAX_DIST_MAP_NUM       (MAX_DIST_MAP_H_NUM * MAX_DIST_MAP_V_NUM)
#define DS_1E4                 10000
#define DS_PDAF_CONF_THRESH    200



/** depth_service_type_t:
 *
 * Enum to distinguish differnt AF technology Input
 *
 **/
typedef enum {
  DEPTH_SERVICE_PDAF = 1,
  DEPTH_SERVICE_TOF,
  DEPTH_SERVICE_DUAL_CAM_AF
} depth_service_type_t;

/* Struct definition */
/** depth_service_roi_t:
 *    @x:   horizon tal offset of the region
 *    @y:   vertical offset of the region
 *    @dx:  width of the RoI region
 *    @dy:  height of the region
 *    @roi: structure holding dimensions of each ROI
 *
 * RoI information in terms of "CAMIF"
 **/
typedef struct _depth_service_roi_t {
  uint16_t x;
  uint16_t y;
  uint16_t dx;
  uint16_t dy;
} depth_service_roi_t;

/** depth_service_af_info_t:
 *    @af_focus_pos: Current AF position based on logical idx
 *
 **/
typedef struct _depth_service_af_info_t {
  int32_t  af_focus_pos_dac;
} depth_service_af_info_t;

/** depth_service_aec_info_t:
 *    @lux_idx: Current lux index
 *    @real_gain: Current gain
 *    @line_count: Current line count
 *
 **/
typedef struct _depth_service_aec_info_t {
  float lux_idx;
  float real_gain;
  int32_t line_count;
} depth_service_aec_info_t;

/** depth_service_awb_info_t:
 *    @gain: R,G and B gains
 *    @color_temp: Current color temperature
 *
 **/
typedef struct _depth_service_awb_info_t {
  awb_gain_t gain;
  uint32_t color_temp;
  int decision;
} depth_service_awb_info_t;

/** depth_service_gyro_t:
 *    @float_ready: float data is available
 *    @flt:         gyro metrics in float
 *    @q16_ready:   gyro metrics in q16 ready to use
 *    @q16:         gyro metrics in q16
 *
 * Gyro data required to assist AF.
**/
typedef struct _depth_service_gyro_t {
  int   float_ready;
  float flt[3];
  int   q16_ready;
  long  q16[3];
} depth_service_gyro_t;

/** depth_service_gravity_t
 *  @gravity:  gravity vector (x/y/z) in m/s/s
 *  @lin_accel: linear acceleration (x/y/z) in m/s/s
 *  @accuracy: sensor accuracy (unreliable/low/medium/high)
 *
 *  This structure is used to store and trasnmit gravity
 *  vector received from the motion sensor.
 *
 **/
typedef struct _depth_service_gravity_t {
  float gravity[3];
  float lin_accel[3];
  uint8_t accuracy;
} depth_service_gravity_t;


/** depth_service_af_info_t:
 *
 *    @frame_id : Frame id associated to each SOF
 *    @type: Determine which AF technology is used
 *    @grid_roi: roi of depth map based on CAMIF
 *    @x_win_num: Number of horizontal regions
 *    @y_win_num: Number of vertical regions
 *
 * Generic info which needs to be copes
 **/
typedef struct _depth_service_info_t{
  uint32_t frame_id;
  depth_service_type_t type;
  depth_service_roi_t grid_roi;
  uint8_t x_win_num;
  uint8_t y_win_num;
  depth_service_af_info_t af;
  depth_service_aec_info_t aec;
  depth_service_awb_info_t awb;
  depth_service_gravity_t gravity;
  depth_service_gyro_t gyro;
} depth_service_info_t;

/** _defocus_cal_data_t:
 *
 *    @actuator_travel_btw_cal_pts_um : 0 - invalid
 *    @af_cal_inf_ff: 0 - invalid
 *    @af_cal_macro_ff:  0 - invalid
 *    @af_cal_fd:
 *    @af_cal_ff:
 *    @actuator_tuning_lens_efl:
 *
 * Generic info which needs to be copes
 **/
typedef struct {
boolean is_pdaf_supported;
float af_cal_inf_ff;
float af_cal_inf_fd;
float af_cal_inf_fu;
float eff_focal_length;
float pdaf_cal_offset[MAX_DIST_MAP_NUM];
float pdaf_cal_defocus_coefficient[MAX_DIST_MAP_NUM];
float f_num;
float pixel_size;
float actuator_sensitivity;
} depth_cal_data_t;

/** _depth_service_input_t:
 *
 *    @depth_service_info_t : Information to be copied to output
 *    @pdaf_info: Sensor output from PDAF library
 *
 * Input to Depth service layer
 **/
typedef struct _depth_service_input_t{
  depth_service_info_t info;
  depth_cal_data_t cal_data;
  union {
    pdaf_update_t pdaf_info;
    tof_update_t  tof_info;
    //dual_cam_af_update_t dual_cam_af_info;
  } u;
} depth_service_input_t;

/** af_tof_update_t:
 *
 *  @frame_id : current frame id, can be optional since it relies on timestamp
 *  @timestamp : timestamp of arrival of the laser data
 *  @valid_data: flag to indicate if tof data is valid or not
 *  @range_mm: flag to indicate laser data is valid or not
 *  @signalRate_mcps: signal rate (MCPS)\n these is a 9.7 fix point value,
 *   which is effectively a measure of target reflectance.
 *  @errorStatus: Error status of the current measurement. \n
 *  @DMaxSq: DMax²  when applicable
 *  @rtnAmbRate: Return Ambient rate related to RESULT_RANGE_RETURN_AMB_COUNT
 *  @rtnConvTime: Return Convergence time  RESULT_RANGE_RETURN_CONV_TIME
 *
 **/
typedef struct {
  uint32_t frame_id;
  struct timeval timestamp;
  int32_t  valid_data;
  int32_t  range_mm;
  int32_t  signalRate_mcps;
  uint32_t errorStatus;
  uint32_t DMaxSq;
  uint32_t rtnAmbRate;
  uint32_t rtnConvTime;
} af_tof_update_t;


/** _depth_service_output_t:
 *
 *    @info: Pointer to input info
 *    @depth_map: Distance map for each region
 *    @confidence_map: Boolean Confidence level for each region
 *    @tolerance_near: Near tolerance level
 *    @tolerance_far: Far tolerance level
 *    @ready: Is data ready
 *
 * Output data from Depth service layer.
 **/
typedef struct _depth_service_output_t{
  depth_service_input_t input;
  float depth_map[MAX_DIST_MAP_H_NUM][MAX_DIST_MAP_V_NUM];
  uint8_t confidence_map[MAX_DIST_MAP_H_NUM][MAX_DIST_MAP_V_NUM];
  float tolerance_near[MAX_DIST_MAP_H_NUM][MAX_DIST_MAP_V_NUM];
  float tolerance_far[MAX_DIST_MAP_H_NUM][MAX_DIST_MAP_V_NUM];
  boolean is_ready;
  af_tof_update_t af_tof_update;
} depth_service_output_t;


/* API function(s) */

boolean depth_service_process(depth_service_input_t *input,
  depth_service_output_t *output);
/* Utility fns*/
boolean depth_service_scale_cal_data(float *input, uint16_t input_size,
  float *output, uint8_t out_size);
#endif /* DEPTH_SERVICE_H_ */
