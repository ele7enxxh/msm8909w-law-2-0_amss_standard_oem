/* depth_service.c
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "depth_service.h"
#include "math.h"
#include "camera_dbg.h"

#undef  LOG_TAG
#define LOG_TAG "DEPTH_SERVICE"

#if !defined MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif
#define DS_GRAVITY_VECTOR 9.81
/** depth_service_calc_hypderfocal_lens_pos:
 *    @input: Input of Depth Service layer
 *    @
 *
 * Function to calculate Lens position at hyperfocal focus.
 *
 * Return hyperfocus distance
 **/
float depth_service_calc_hypderfocal_lens_pos(depth_service_input_t *input)
{
  depth_cal_data_t *cal_data = &(input->cal_data);
  float hyp_lens_pos_dac = cal_data->af_cal_inf_ff;
  float gravity_vector_z = input->info.gravity.gravity[2]/ DS_GRAVITY_VECTOR;
  /* check for tan 90 */
  if (input->info.gyro.q16_ready == TRUE && gravity_vector_z != 90) {
    if (gravity_vector_z > 0) {
      hyp_lens_pos_dac += (tan(gravity_vector_z) *
        abs(cal_data->af_cal_inf_fu  - cal_data->af_cal_inf_ff));
    } else {
      hyp_lens_pos_dac += (tan(gravity_vector_z) *
        abs(cal_data->af_cal_inf_fd  - cal_data->af_cal_inf_ff));
    }
  }
  return hyp_lens_pos_dac;
}
/** depth_service_scale_cal_data:
 *    @input: Input buffer
 *    @input_size: size of array element
 *    @output: Output buffer
 *    @
 *
 * Function to convert Raw pdaf data to Depth service output
 *
 * Return boolean.
 **/
boolean depth_service_scale_cal_data(float *input, uint16_t input_size,
  float *output, uint8_t out_size)
{
  uint16_t i;
  if(input_size == 0) {
    CDBG_ERROR("Depth_Err %s: Input size is 0", __func__);
    return FALSE;
  }
  float ratio = out_size / input_size;
  if (ratio != 4) {
    CDBG_ERROR("Depth_Err %s: Ratio should be 4 ", __func__);
    return FALSE;
  }
  if (out_size > MAX_DIST_MAP_NUM) {
    CDBG_ERROR("Depth_Err %s: out_size %d > %d", __func__,
      out_size, MAX_DIST_MAP_NUM);
    return FALSE;
  }
  output[0] =  input[0];
  output[out_size - 1] =  input[input_size - 1];
  for (i = 1; i < out_size; i++) {
    if ((i % 2) == 0) {
      output[i] =  input[i];
      output[i - 1] =  (input[i - 2] + input[i]) / 2;
    }
  }
  return TRUE;
}

/** depth_service_handle_dual_cam:
 *    @input: Input to depth service layer
 *    @output: Output to depth service layer
 *
 * Function to convert Raw Dual cam data to Depth service output
 *
 * Return boolean.
 **/
boolean depth_service_handle_dual_cam(depth_service_input_t *input,
  depth_service_output_t *output)
{
  float lens_eqn_x0_cm, lens_eqn_x1_cm;
  float lens_dof_cm, near_field_x0_cm, near_field_x1_cm;
  float far_field_x0_cm, far_field_x1_cm;
  float focal_sq_cm;
  depth_cal_data_t *cal_data = &(input->cal_data);

  /* Precompute to reduce complexity */

  focal_sq_cm = pow((cal_data->eff_focal_length / 10), 2);
  lens_eqn_x1_cm = 1;
  lens_eqn_x0_cm = focal_sq_cm / lens_eqn_x1_cm;

  /* Tolerance map */
  lens_dof_cm = 2 * cal_data->f_num * (cal_data->pixel_size * 2) *
    (lens_eqn_x1_cm / (cal_data->eff_focal_length * 0.1));

  near_field_x0_cm = lens_eqn_x0_cm + (lens_dof_cm / 2);
  near_field_x1_cm = focal_sq_cm / near_field_x0_cm;

  far_field_x0_cm = lens_eqn_x0_cm - (lens_dof_cm / 2);
  far_field_x1_cm = focal_sq_cm / far_field_x0_cm;


  /* Map 1d to 2d array*/
  output->depth_map[0][0] = lens_eqn_x1_cm;
  output->tolerance_near[0][0] = near_field_x1_cm;
  output->tolerance_far[0][0] = far_field_x1_cm;
  output->confidence_map[0][0] = 1;
  CDBG("%s DBG: act_sensi %f eff_focal_len %f \
    curr_lens_pos_dac %d\
    dof %f ; x0, x1 (%f, %f) \
    near x0 x1  (%f %f) far x0 x1 (%f %f)",
    __func__, cal_data->actuator_sensitivity, cal_data->eff_focal_length,
    input->info.af.af_focus_pos_dac,
    lens_dof_cm,lens_eqn_x0_cm, lens_eqn_x1_cm,
    near_field_x0_cm, near_field_x1_cm,
    far_field_x0_cm, far_field_x1_cm);
  return TRUE;
}
/** depth_service_handle_tof:
 *    @input: Input to depth service layer
 *    @output: Output to depth service layer
 *
 * Function to convert Raw tof data to Depth service output
 *
 * Return boolean.
 **/
boolean depth_service_handle_tof(depth_service_input_t *input,
  depth_service_output_t *output)
{
  float lens_eqn_x0_cm, lens_eqn_x1_cm;
  float lens_dof_cm, near_field_x0_cm, near_field_x1_cm;
  float far_field_x0_cm, far_field_x1_cm;
  float focal_sq_cm;
  depth_cal_data_t *cal_data = &(input->cal_data);

  /* Precompute to reduce complexity */

  focal_sq_cm = pow((cal_data->eff_focal_length / 10), 2);
  lens_eqn_x1_cm = 1;
  lens_eqn_x0_cm = focal_sq_cm / lens_eqn_x1_cm;

  /* Tolerance map */
  lens_dof_cm = 2 * cal_data->f_num * (cal_data->pixel_size * 2) *
    (lens_eqn_x1_cm / (cal_data->eff_focal_length * 0.1));

  near_field_x0_cm = lens_eqn_x0_cm + (lens_dof_cm / 2);
  near_field_x1_cm = focal_sq_cm / near_field_x0_cm;

  far_field_x0_cm = lens_eqn_x0_cm - (lens_dof_cm / 2);
  far_field_x1_cm = focal_sq_cm / far_field_x0_cm;

  memcpy(&output->af_tof_update.timestamp,
    &input->u.tof_info.timestamp, sizeof(struct timeval));
  output->af_tof_update.frame_id = input->u.tof_info.frame_id;
  output->af_tof_update.valid_data = input->u.tof_info.valid_data;
  output->af_tof_update.range_mm = input->u.tof_info.range_mm;
  output->af_tof_update.signalRate_mcps = input->u.tof_info.signalRate_mcps;
  output->af_tof_update.errorStatus = input->u.tof_info.errorStatus;
  output->af_tof_update.DMaxSq = input->u.tof_info.DMaxSq;
  output->af_tof_update.rtnAmbRate = input->u.tof_info.rtnAmbRate;
  output->af_tof_update.rtnConvTime = input->u.tof_info.rtnConvTime;

  /*print out all the tof information*/
  CDBG_ERROR("%s: Range %d frame_id %d timestamp %ld.%06ld valid_data %d"
    " Signal_rate %d error_code %d Dmax %d rtn_amb_rate %d,"
    "rtn_conv_time %d", __func__,
    output->af_tof_update.range_mm,
    output->af_tof_update.frame_id,
    output->af_tof_update.timestamp.tv_sec,
    output->af_tof_update.timestamp.tv_usec,
    output->af_tof_update.valid_data,
    output->af_tof_update.signalRate_mcps,
    output->af_tof_update.errorStatus,
    output->af_tof_update.DMaxSq,
    output->af_tof_update.rtnAmbRate,
    output->af_tof_update.rtnConvTime);

  /* Map 1d to 2d array*/
  output->depth_map[0][0] = lens_eqn_x1_cm;
  output->tolerance_near[0][0] = near_field_x1_cm;
  output->tolerance_far[0][0] = far_field_x1_cm;
  output->confidence_map[0][0] = 1;
  CDBG("%s DBG: act_sensi %f eff_focal_len %f \
    curr_lens_pos_dac %d\
    dof %f ; x0, x1 (%f, %f) \
    near x0 x1  (%f %f) far x0 x1 (%f %f)",
    __func__, cal_data->actuator_sensitivity, cal_data->eff_focal_length,
    input->info.af.af_focus_pos_dac,
    lens_dof_cm,lens_eqn_x0_cm, lens_eqn_x1_cm,
    near_field_x0_cm, near_field_x1_cm,
    far_field_x0_cm, far_field_x1_cm);
  return TRUE;
}
/** depth_service_handle_pdaf:
 *    @input: Input to depth service layer
 *    @outptu: Output to depth service layer
 *
 * Function to convert Raw pdaf data to Depth service output
 *
 * Return boolean.
 **/
boolean depth_service_handle_pdaf(depth_service_input_t *input,
  depth_service_output_t *output)
{
  uint16_t i, size, x, y;
  float defocus_grid_dac, target_lens_pos_grid_dac;
  float hyp_lens_pos_dac, lens_eqn_x0_cm, lens_eqn_x1_cm;
  float lens_dof_cm, near_field_x0_cm, near_field_x1_cm;
  float far_field_x0_cm, far_field_x1_cm;
  float focal_sq_cm;
  depth_cal_data_t *cal_data = &(input->cal_data);

  size = input->info.x_win_num * input->info.y_win_num;
  if (size > MAX_DIST_MAP_NUM) {
    CDBG_ERROR("Depth_Err %s: size %d > %d ", __func__, size, MAX_DIST_MAP_NUM);
    return FALSE;
  }
  /* Precompute to reduce complexity*/
  hyp_lens_pos_dac = depth_service_calc_hypderfocal_lens_pos(input);
  focal_sq_cm = pow((cal_data->eff_focal_length / 10), 2);

  /* Convert Raw PDAF data to Depth service output */
  for (i = 0; i < size; i++) {
    /* To be enabled for PDAF 3*/
    #if 0
    defocus_grid_dac = (input->u.pdaf_info.defocus[i].phase_diff - cal_data->pdaf_cal_offset[i]) *
      cal_data->pdaf_cal_defocus_coefficient[i];
    #endif
    defocus_grid_dac = input->u.pdaf_info.defocus[i].defocus;
    target_lens_pos_grid_dac = (defocus_grid_dac + input->info.af.af_focus_pos_dac) *
      cal_data->actuator_sensitivity;
      /* Sanity to cap target_lens_pos*/
      target_lens_pos_grid_dac = MAX(target_lens_pos_grid_dac, hyp_lens_pos_dac);

     /* Compute depth map in cm*/
     lens_eqn_x0_cm = (abs(hyp_lens_pos_dac - target_lens_pos_grid_dac) * cal_data->actuator_sensitivity) / DS_1E4;
     lens_eqn_x1_cm = focal_sq_cm / lens_eqn_x0_cm;

     /* Tolerance map */
     lens_dof_cm = 2 * cal_data->f_num * (cal_data->pixel_size * 2) *
       (lens_eqn_x1_cm / (cal_data->eff_focal_length * 0.1));

     near_field_x0_cm = lens_eqn_x0_cm + (lens_dof_cm / 2);
     near_field_x1_cm = focal_sq_cm / near_field_x0_cm;

     far_field_x0_cm = lens_eqn_x0_cm - (lens_dof_cm / 2);
     far_field_x1_cm = focal_sq_cm / far_field_x0_cm;


     /* Convert 1d to 2d array*/
     x = i / input->info.x_win_num;
     y = i % input->info.y_win_num;
     output->depth_map[x][y] = lens_eqn_x1_cm;
     output->tolerance_near[x][y] = near_field_x1_cm;
     output->tolerance_far[x][y] = far_field_x1_cm;
     if(input->u.pdaf_info.defocus[i].df_conf_level < DS_PDAF_CONF_THRESH)
       output->confidence_map[x][y] = 0;
     else
       output->confidence_map[x][y] = 1;

     /* Print center grid Debug data */
     if(i == ((input->info.x_win_num - 1)* (input->info.y_win_num/2))) {
       CDBG("%s DBG: act_sensi %f hyp_focal %f eff_focal_len %f \
         defocus_dac %f confidence %d targ_lens_pos_dac  %f  curr_lens_pos_dac %d\
         dof %f ; x0, x1 (%f, %f) \
         near x0 x1  (%f %f) far x0 x1 (%f %f)",
         __func__, cal_data->actuator_sensitivity,hyp_lens_pos_dac, cal_data->eff_focal_length,
         defocus_grid_dac,input->u.pdaf_info.defocus[i].df_conf_level,target_lens_pos_grid_dac, input->info.af.af_focus_pos_dac,
         lens_dof_cm,lens_eqn_x0_cm, lens_eqn_x1_cm,
         near_field_x0_cm, near_field_x1_cm,
         far_field_x0_cm, far_field_x1_cm);
     }
  }
  output->is_ready = TRUE;
  return TRUE;
}

/** depth_service_process:
 *    @input: Input to depth service layer
 *    @outptu: Output to depth service layer
 *
 * Entry point to handle Depth service request
 *
 * Return boolean.
 **/
boolean depth_service_process(depth_service_input_t *input,
  depth_service_output_t *output)
{
  boolean rc = FALSE;
  /* Sanitize */
  if (!input || !output)
     return rc;

  /* Handle AF tech type*/
  switch(input->info.type) {
    case DEPTH_SERVICE_PDAF: {
      rc = depth_service_handle_pdaf(input, output);
    }
    break;
  case DEPTH_SERVICE_TOF: {
    rc = depth_service_handle_tof(input, output);
  }
  break;
  case DEPTH_SERVICE_DUAL_CAM_AF: {
    rc = depth_service_handle_dual_cam(input, output);
  }
  break;
    default: {
      CDBG_ERROR("Depth_Err %s:Invalid input type to Dist service layer %d",
        __func__, input->info.type);
    }
  } /* switch E*/
  if(rc) {
    /* Map common info from input to output*/
    output->input = *input;
  }
  return rc;
}
