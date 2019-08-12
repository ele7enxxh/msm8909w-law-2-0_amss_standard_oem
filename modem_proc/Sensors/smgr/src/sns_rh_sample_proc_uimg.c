/*=============================================================================
  @file sns_rh_sample_proc_uimg.c

  This file implements the sample processing functions for the Request Handler

******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_sample_proc_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-12-04  ad   Fix applying of factory and auto-calibration parameters 
  2015-09-10  pn   Improved interpolation implementation
  2015-04-09  pn   Added support for 56-bit timestamp
  2015-01-22  BD   Changed the way of putting functions in uImage to use sections
  2014-11-10  jc   Changed accumulator type to float to avoid overflow
  2014-10-25  sd   modified interpolation function 
  2014-10-15  ks   Added mag anomaly detector, multi accuracy level support
  2014-07-20  VY   Disabled logging in uImage
  2014-06-16  pn   Avoided unnecessary recalculation of CIC timestamp adjustment 
  2014-06-16  pn   Removed references to unused circular buffer
  2014-06-11  vy   Further refactored to support uImage  
  2014-04-29  pn   Initial version (renamed from sns_rh_sample_proc.c)

============================================================================*/

#define __SNS_MODULE__ SNS_RH

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_memmgr.h"
#include "sns_log_api.h"

#include "sns_rh_sample_proc_uimg.h"
#include "sns_rh_sol.h"
#include "sns_smgr.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"
#include "sns_rh_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define SNS_SMGR_SAMPLE_PROC_DEBUG
#ifdef SNS_SMGR_SAMPLE_PROC_DEBUG
#define SNS_SMGR_SAMPLE_PROC_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_SAMPLE_PROC_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_SAMPLE_PROC_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_SAMPLE_PROC_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_SAMPLE_PROC_DBG4 SNS_SMGR_PRINTF4
#else
#define SNS_SMGR_SAMPLE_PROC_DBG0(level,msg)
#define SNS_SMGR_SAMPLE_PROC_DBG1(level,msg,p1)
#define SNS_SMGR_SAMPLE_PROC_DBG2(level,msg,p1,p2)
#define SNS_SMGR_SAMPLE_PROC_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_SAMPLE_PROC_DBG4(level,msg,p1,p2,p3,p4)
#endif

//#define SNS_SMGR_INTERPOLATOR_DEBUG
#ifdef SNS_SMGR_INTERPOLATOR_DEBUG
#define SNS_SMGR_INTERP_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_INTERP_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_INTERP_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_INTERP_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_INTERP_DBG4 SNS_SMGR_PRINTF4
#define SNS_SMGR_INTERP_DBG5 SNS_SMGR_PRINTF5
#else
#define SNS_SMGR_INTERP_DBG0(level,msg)
#define SNS_SMGR_INTERP_DBG1(level,msg,p1)
#define SNS_SMGR_INTERP_DBG2(level,msg,p1,p2)
#define SNS_SMGR_INTERP_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_INTERP_DBG4(level,msg,p1,p2,p3,p4)
#define SNS_SMGR_INTERP_DBG5(level,msg,p1,p2,p3,p4,p5)
#endif

#define SNS_SMGR_CAL_ACCURACY_UNRELIABLE        (0)
#define SNS_SMGR_CAL_ACCURACY_LOW               (1)
#define SNS_SMGR_CAL_ACCURACY_MEDIUM            (2)
#define SNS_SMGR_CAL_ACCURACY_HIGH              (3)

#define SNS_SMGR_MAG_ANOMALY_DET_THRESH_GAUSS_SQ  (2.0f*2.0f)

/*----------------------------------------------------------------------------
 * Macros
 * -------------------------------------------------------------------------*/
#define RH_CALIBRATE_BIAS_AND_GAIN(in, factor, bias) (FX_MUL_Q16(in,factor)+(bias))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*******************************************
 CIC FILTER OBJECT AND FUNCTIONS
********************************************/

/*===========================================================================

  FUNCTION:   sns_rh_cic_reset

===========================================================================*/
/*!
  @brief Reset CIC filter state

  @detail
  - Prepare a filter for its first input
  - Zero accumulator and count, store the decimation factor and axis_cnt.
  - axis_cnt <= SMGR_MAX_VALUES_PER_DATA_TYPE. May be truncated without warning

  @param[update] cic_obj: Ptr to the CIC filter state structure
  @param[in] factor: Decimation factor, must be >0, >= 2 for practical uses
  @param[in] axis_cnt: Number of axes (example 3 for XYZ)

  @return
    void
*/
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC void sns_rh_cic_reset(sns_rh_cic_obj_s* cic_obj_ptr)
{
  SNS_OS_MEMZERO(cic_obj_ptr->accumulator, sizeof(cic_obj_ptr->accumulator));
  cic_obj_ptr->status = SNS_DDF_SUCCESS;
  cic_obj_ptr->count  = 0;
}

/*===========================================================================

  FUNCTION:   sns_rh_cic_update

===========================================================================*/
/*!
  @brief Update CIC filter

  @detail
  - Accepts input data, outputs filtered data every <factor> entry
  - Input data samples must be equally spaced in time, within small tolerance
  - Filter state must be initialized with sns_smgr_cic_init() before starting
    a series of updates. If used without init, update guards against fatal
    error, but output is meaningless.

  @param[in/out] cic_obj_ptr: Ptr to the CIC filter state structure
  @param[in] input_ptr: input sample
  @param[out] timestamp_ptr: destination for resulting timestamp
  @param[out] dest_data: Ptr to array of filtered data

  @return
    SNS_DDF_SUCCESS or
    SNS_DDF_EINVALID_DATA - output has been populated (every <factor> entry)

    SNS_DDF_PENDING - input has updated cic_obj - no output

  $TODO:
*/
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC sns_ddf_status_e sns_rh_cic_update(
  sns_rh_cic_obj_s*         cic_obj_ptr,
  const sns_smgr_sample_s*  input_ptr,
  sns_ddf_time_t*           timestamp_ptr,
  int32_t                   dest_data[SNS_SMGR_SENSOR_DIMENSION_V01])
{
  uint32_t  i;
  sns_ddf_status_e status = SNS_DDF_PENDING;
  sns_ddf_status_e sample_status = SMGR_SAMPLE_STATUS(input_ptr);
  sns_ddf_time_t   sample_timestamp = SMGR_SAMPLE_TIMESTAMP(input_ptr);
  /* -------------------------------------------------------------------- */

  SNS_SMGR_SAMPLE_PROC_DBG3(
    LOW, "cic_update - ts=%u status=%u count=%d",
    sample_timestamp, sample_status, cic_obj_ptr->count);

  /* Guard against uninitialized axis_cnt */
  if ( SMGR_MAX_VALUES_PER_DATA_TYPE >= cic_obj_ptr->axis_cnt )
  {
    /* Sum axes into their accumulators */
    for ( i = 0; i < cic_obj_ptr->axis_cnt ; i++ )
    {
      /* Sum input into accumulator in float format to avoid overflow */
      cic_obj_ptr->accumulator[i] += FX_FIXTOFLT_Q16_SP(input_ptr->data[i]);
    }
    if ( sample_status != SNS_DDF_SUCCESS )
    {
      cic_obj_ptr->status = sample_status;
    }

    /* Tally count. Count can never be >factor, but prevent runaway if it
      happens. Prevent zero divide if filter is run without initialization */
    if ( (++(cic_obj_ptr->count)) >= cic_obj_ptr->factor &&
         (0 != cic_obj_ptr->factor) )
    {
      if ( cic_obj_ptr->status == SNS_DDF_SUCCESS )
      {
        for ( i=0; i<cic_obj_ptr->axis_cnt; i++)
        {
          dest_data[i] = FX_FLTTOFIX_Q16(cic_obj_ptr->accumulator[i] 
                         / (float)cic_obj_ptr->factor);
        }
      }
      *timestamp_ptr = sample_timestamp - cic_obj_ptr->delay_ticks;
      status = cic_obj_ptr->status;

      sns_rh_cic_reset(cic_obj_ptr);

      SNS_SMGR_SAMPLE_PROC_DBG3(
        MED, "cic_update - in_ts=%u out_ts=%u status=%u",
        sample_timestamp, *timestamp_ptr, status);
    }
  }
  return status;
}


/*******************************************
 INTERPOLATOR OBJECT AND FUNCTIONS
********************************************/
/*===========================================================================

  FUNCTION:   sns_rh_interpolator_update

===========================================================================*/
/*!
  @brief Shifts samples so that sample1 is older than sample2

  @param[io] int_obj_ptr - interpolator object
  @param[i]  sample_ptr - the new sample

  @return none
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE void sns_rh_interpolator_update(
  sns_rh_interpolator_obj_s*  int_obj_ptr,
  const sns_smgr_sample_s*    sample_ptr)
{
  sns_rh_rpt_spec_s* rpt_ptr = int_obj_ptr->item_ptr->parent_report_ptr;

  if ( sample_ptr != NULL )
  {
    /* shifts the samples s1 <- s2 <- new */
    int_obj_ptr->sample1_ptr  = int_obj_ptr->sample2_ptr;
    int_obj_ptr->sample2_ptr  = sample_ptr;

    if ( int_obj_ptr->sample1_ptr != NULL ) /* both s1 and s2 are present? */
    {
      sns_ddf_time_t ts2 = SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample2_ptr);
      sns_ddf_time_t ts1 = SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample1_ptr);

      if ( TICK1_GEQ_TICK2(ts2, ts1) ) /* s1 older than s2 - the normal case */
      {
        if ( TICK1_GTR_TICK2(ts1, int_obj_ptr->desired_timestamp) )
        {
          /* desired timestamp is in the past hence not producible */
          SNS_SMGR_PRINTF4(
            ERROR, "interp_update - rpt_id=%u/0x%x desired(old/new)=%u/%u", 
            rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle,
            int_obj_ptr->desired_timestamp, ts1);

          /* starts from s1 */
          int_obj_ptr->desired_timestamp = ts1;
        }
      }
      else /* depot overflowed since last access */
      {
        /* restarting with s2 */
        int_obj_ptr->sample1_ptr = NULL;
      }
    }
    SNS_SMGR_INTERP_DBG5(
      LOW, "interp_update - id=%u/0x%x ts1=%u ts=%u ts2=%u",
      rpt_ptr->rpt_id, rpt_ptr->msg_header.connection_handle,
      int_obj_ptr->sample1_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample1_ptr) : 0,
      int_obj_ptr->desired_timestamp,
      int_obj_ptr->sample2_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample2_ptr) : 0);
  }
  else
  {
    SNS_SMGR_PRINTF3(
      ERROR, "interp_update - ts1=%u desired=%u ts2=%u",
      int_obj_ptr->sample1_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample1_ptr) : 0,
      int_obj_ptr->desired_timestamp,
      int_obj_ptr->sample2_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample2_ptr) : 0);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_interpolate

===========================================================================*/
/*!
  @brief Interpolates a new sample between two known samples.

  @details Upon entering this function desired_timestamp field of the
  given interpolator object would hold the desired timestamp for the
  resulting sample.  Successful interpolation would result in a valid sample
  saved in the given destination.

  @param[i]  smgr_interpolator_obj_s - object holding data needed for interpolation
  @param[io] dest_data - destination for the resulting sample

  @return
  SNS_DDF_SUCCESS        - if a new sample was created
  SNS_DDF_EINVALID_PARAM - if one or more of the input pointers are NULL
  SNS_DDF_EINVALID_DATA  - if the desired timestamp is not between the timestamps
                           of given samples
*/
/*=========================================================================*/
SNS_RH_UIMAGE_CODE sns_ddf_status_e sns_rh_interpolate(
  sns_rh_interpolator_obj_s*  int_obj_ptr,
  int32_t                     dest_sample[SNS_SMGR_SENSOR_DIMENSION_V01])
{
  sns_ddf_status_e err = SNS_DDF_EINVALID_PARAM;
  sns_ddf_time_t ts1 = 
    int_obj_ptr->sample1_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample1_ptr) : 0;
  sns_ddf_time_t ts2 = 
    int_obj_ptr->sample2_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample2_ptr) : 0;

  if ( (dest_sample != NULL) && (ts1 != 0) && (ts2 != 0) &&
       /* make sure timestamp of interpolated sample is between timestamps of
          the two known samples */
       TICK1_GEQ_TICK2(int_obj_ptr->desired_timestamp, ts1) &&
       TICK1_GEQ_TICK2(ts2, int_obj_ptr->desired_timestamp) )
  {
    uint8_t i;
    sns_ddf_time_t tsgap_smp1_smp2 = ts2 - ts1;

    err = SMGR_SAMPLE_STATUS(int_obj_ptr->sample1_ptr) |
          SMGR_SAMPLE_STATUS(int_obj_ptr->sample2_ptr);

    if ( tsgap_smp1_smp2 == 0 )
    {
      /* this is the case of first interpolated sample generated from one raw sample */
      for ( i=0; i<SMGR_MAX_VALUES_PER_DATA_TYPE; i++) 
      { 
        dest_sample[i] = int_obj_ptr->sample1_ptr->data[i]; 
      } 
    }
    else
    {
      q16_t factor;
      sns_ddf_time_t tsgap_smp_smp1 = int_obj_ptr->desired_timestamp - ts1;
      factor = FX_FLTTOFIX_Q16((float)tsgap_smp_smp1 / (float)tsgap_smp1_smp2);

      SNS_SMGR_INTERP_DBG3(
        MED, "interpolate - t-t1=%u t2-t1=%u factor=0x%x",
        tsgap_smp_smp1, tsgap_smp1_smp2, factor);

      for ( i=0; i<SMGR_MAX_VALUES_PER_DATA_TYPE; i++ )
      {
        q16_t delta =
          int_obj_ptr->sample2_ptr->data[i] - int_obj_ptr->sample1_ptr->data[i];
        dest_sample[i] = 
          int_obj_ptr->sample1_ptr->data[i] + FX_MUL_Q16(delta, factor);
        SNS_SMGR_INTERP_DBG3(
          LOW, "interpolate - s1=0x%x s2=0x%x dest=0x%x",
          int_obj_ptr->sample1_ptr->data[i], int_obj_ptr->sample2_ptr->data[i], 
          dest_sample[i]);
      }
    }
  }
  return err;
}


/*******************************************
 CALIBRATION UTILITIES
********************************************/
/*===========================================================================

  FUNCTION:   sns_rh_calibrate_cm_and_bias

===========================================================================*/
/*!
  @brief 

  @detail

  @param[io]  sample
  @param[i]   cm compensation matrix
  @param[i]   zb zero bias
 
  @return     None
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE SMGR_STATIC void sns_rh_calibrate_cm_and_bias(
  int32_t *sample, 
  int32_t *cm, 
  int32_t *zb)
{
  q16_t temp[SNS_SMGR_SENSOR_DIMENSION_V01];
  uint32_t a, b;
  /* -------------------------------------------------------------------- */

  for ( a=0; a<ARR_SIZE(temp); a++ )
  {
    /* Get rid of bias, Note for legacy reasons, bias is additive*/
    temp[a]   = sample[a] + zb[a];
    sample[a] = 0;
  }
  for ( a=0; a<ARR_SIZE(temp); a++ )
  {
    uint32_t c = a*SNS_SMGR_SENSOR_DIMENSION_V01;
    for ( b=0; b<SNS_SMGR_SENSOR_DIMENSION_V01; b++ )
    {
      /* Apply CM */
      sample[a] += FX_MUL_Q16(cm[c+b], temp[b]);
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_log_calibration

===========================================================================*/
/*!
  @brief calibrate the sampled data and save them into their saving location

  @Detail

  @param[i] sensor_ptr  A sensor leader sturcture that holds information for a sensor
  @param[i] data_type Primary or Secondary

  @return
   NONE
*/
/*=========================================================================*/
SNS_RH_UIMAGE_CODE static void sns_rh_log_calibration(
  q16_t                   sample_data[],
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr)
{
  sns_smgr_cal_s* cal_ptr = &ddf_sensor_ptr->all_cal_ptr->auto_cal;
  if ( !cal_ptr->need_to_log || 
       SNS_SMGR_DATA_TYPE_PRIMARY_V01 != ddf_sensor_ptr->data_type)
  {
    return;
  }

  if ( SMGR_SENSOR_IS_GYRO(ddf_sensor_ptr->sensor_ptr) )
  {
    /* gyro logpacket */
    sns_err_code_e retval;
    sns_log_gyro_cal_report_s* log_struct_ptr;
    retval = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_REPORT,
               sizeof(sns_log_gyro_cal_report_s),(void**)&log_struct_ptr);
    if ( retval == SNS_SUCCESS )
    {
      log_struct_ptr->version = SNS_LOG_GYRO_CAL_REPORT_VERSION;
      log_struct_ptr->timestamp = sns_ddf_get_timestamp();
      log_struct_ptr->gyro_x = sample_data[0];
      log_struct_ptr->gyro_y = sample_data[1];
      log_struct_ptr->gyro_z = sample_data[2];
      log_struct_ptr->gyro_zero_bias_corr_x = cal_ptr->zero_bias[0];
      log_struct_ptr->gyro_zero_bias_corr_y = cal_ptr->zero_bias[1];
      log_struct_ptr->gyro_zero_bias_corr_z = cal_ptr->zero_bias[2];
      //Commit log (also frees up the log packet memory)
      retval = sns_logpkt_commit(SNS_LOG_GYRO_CAL_REPORT,log_struct_ptr);
    }
    /* clear the flag at any case */
    cal_ptr->need_to_log = FALSE;
  }
  else if ( SMGR_SENSOR_IS_MAG(ddf_sensor_ptr->sensor_ptr) ) 
  {
    sns_err_code_e err;
    sns_log_qmag_cal_report_s* log_qmag_struct_ptr;   
    err = sns_logpkt_malloc(SNS_LOG_QMAG_CAL_REPORT,
                 sizeof(sns_log_qmag_cal_report_s),(void**)&log_qmag_struct_ptr);

    if ( err == SNS_SUCCESS )
    {
      log_qmag_struct_ptr->version = SNS_LOG_QMAG_CAL_REPORT_VERSION;
      log_qmag_struct_ptr->timestamp_type = SNS_TIMESTAMP_DSPS;
      log_qmag_struct_ptr->timestamp = sns_ddf_get_timestamp();
      log_qmag_struct_ptr->qmag_x = sample_data[0];
      log_qmag_struct_ptr->qmag_y = sample_data[1];
      log_qmag_struct_ptr->qmag_z = sample_data[2];
      log_qmag_struct_ptr->qmag_zero_bias_corr_x = cal_ptr->zero_bias[0];
      log_qmag_struct_ptr->qmag_zero_bias_corr_y = cal_ptr->zero_bias[1];
      log_qmag_struct_ptr->qmag_zero_bias_corr_z = cal_ptr->zero_bias[2];
      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QMAG_CAL_REPORT,log_qmag_struct_ptr);
    }
    /* clear the flag at any case */
    cal_ptr->need_to_log = false;
  }
}


/*===========================================================================

  FUNCTION:   smgr_apply_calibration

===========================================================================*/
/*!
  @brief This function apply the calibratin data into the sampled data

  @detail

  @param[io]  sample_data the sampled data to be calibrated
  @param[i]   ddf_sensor_ptr the sensor whose calibration data is to be used
  @param[i]   cal_sel calibration selection flag
 
  @return     The calibration applied flags
 */
/*=========================================================================*/
SNS_RH_UIMAGE_CODE uint8_t sns_rh_apply_calibration(
  q16_t                   sample_data[],
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr,
  uint8_t                 cal_sel)
{
  uint8_t  cal_applied_flags = 0;
  uint32_t i;
  sns_smgr_all_cal_s* all_cal_ptr = ddf_sensor_ptr->all_cal_ptr;
  sns_smgr_cal_s* cal_data_ptr = NULL;
  /* -------------------------------------------------------------------- */

  if ( all_cal_ptr == NULL || cal_sel == SNS_SMGR_CAL_SEL_RAW_V01 )
  {
    return cal_applied_flags;
  }

  if ( (SNS_SMGR_CAL_SEL_FULL_CAL_V01 == cal_sel) &&
       all_cal_ptr->full_cal.used )
  {
    cal_data_ptr = &all_cal_ptr->full_cal;
    if ( all_cal_ptr->factory_cal.used )
    {
      cal_applied_flags |= SNS_SMGR_ITEM_FLAG_FAC_CAL_V01;
    }
    if ( all_cal_ptr->auto_cal.used )
    {
      cal_applied_flags |= SNS_SMGR_ITEM_FLAG_AUTO_CAL_V01;
    }
  }
  else if ( (SNS_SMGR_CAL_SEL_FACTORY_CAL_V01 == cal_sel) &&
            all_cal_ptr->factory_cal.used )
  {
    cal_data_ptr = &all_cal_ptr->factory_cal;
    if ( all_cal_ptr->factory_cal.used )
    {
      cal_applied_flags |= SNS_SMGR_ITEM_FLAG_FAC_CAL_V01;
    }
  }
  if ( NULL != cal_data_ptr )
  {
    uint8_t accuracy = SNS_SMGR_CAL_ACCURACY_UNRELIABLE;

    if ( cal_data_ptr->need_to_log )
    {
      sns_rh_log_calibration(sample_data, ddf_sensor_ptr);
      cal_data_ptr->need_to_log = FALSE;
    }

    // Apply factory calibration parameters if applicable
    if (cal_applied_flags & SNS_SMGR_ITEM_FLAG_FAC_CAL_V01)
    {
      if (all_cal_ptr->factory_cal.compensation_matrix_valid)
      {
        /* Compensation Matrix and bias*/
        sns_rh_calibrate_cm_and_bias(sample_data, 
                                     all_cal_ptr->factory_cal.compensation_matrix, 
                                     all_cal_ptr->factory_cal.zero_bias);
      }
      else
      {
        /* Only Bias and Scale Factor */
        for ( i=0; i < SNS_SMGR_SENSOR_DIMENSION_V01 ; i++ )
        {
          sample_data[i] = RH_CALIBRATE_BIAS_AND_GAIN(sample_data[i],
                                                      all_cal_ptr->factory_cal.scale_factor[i],
                                                      all_cal_ptr->factory_cal.zero_bias[i]);
        }
      }
    }
    // Apply auto-calibration parameters if applicable
    if (cal_applied_flags & SNS_SMGR_ITEM_FLAG_AUTO_CAL_V01)
    {
      if (all_cal_ptr->auto_cal.compensation_matrix_valid)
    {
      /* Compensation Matrix and bias*/
      sns_rh_calibrate_cm_and_bias(sample_data, 
                                     all_cal_ptr->auto_cal.compensation_matrix, 
                                     all_cal_ptr->auto_cal.zero_bias);
    }
    else
    {
      /* Only Bias and Scale Factor */
      for ( i=0; i < SNS_SMGR_SENSOR_DIMENSION_V01 ; i++ )
      {
        sample_data[i] = RH_CALIBRATE_BIAS_AND_GAIN(sample_data[i],
                                                      all_cal_ptr->auto_cal.scale_factor[i],
                                                      all_cal_ptr->auto_cal.zero_bias[i]);
        }
      }
    }

    if ( SMGR_SENSOR_IS_MAG(ddf_sensor_ptr->sensor_ptr) )
    {
      if ( cal_data_ptr->calibration_accuracy != SNS_SMGR_CAL_ACCURACY_UNRELIABLE )
      {
        // Mag Anomaly Detector
        float cal_mag_norm_sq = 0.0, mag_sample[SNS_SMGR_SENSOR_DIMENSION_V01];

        mag_sample[0] = FX_FIXTOFLT_Q16(sample_data[0]);
        mag_sample[1] = FX_FIXTOFLT_Q16(sample_data[1]);
        mag_sample[2] = FX_FIXTOFLT_Q16(sample_data[2]);

        cal_mag_norm_sq = mag_sample[0]*mag_sample[0] +
          mag_sample[1]*mag_sample[1] + mag_sample[2]*mag_sample[2];

        if (cal_mag_norm_sq > (SNS_SMGR_MAG_ANOMALY_DET_THRESH_GAUSS_SQ))
        {
          // reset saved cal accuracy since anomaly may have caused
          // permanent magnetization
          cal_data_ptr->calibration_accuracy = SNS_SMGR_CAL_ACCURACY_UNRELIABLE;
          SNS_SMGR_PRINTF0(HIGH, "smgr mag anomaly detected");
        }
      }

      if (cal_data_ptr->calibration_accuracy <= SNS_SMGR_CAL_ACCURACY_HIGH)
      {
        accuracy = (uint8_t)(cal_data_ptr->calibration_accuracy);
      }
      else
      {
        SNS_SMGR_PRINTF1(ERROR, "unsupported cal accuracy requested: %d",
                         cal_data_ptr->calibration_accuracy);
      }
    }
    else
    {
      accuracy = SNS_SMGR_CAL_ACCURACY_HIGH;
    }
    accuracy <<= 1; // bits 1,2 of ItemFlags used to convey accuracy
    cal_applied_flags |= accuracy;
  }
  return cal_applied_flags;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_sample

===========================================================================*/
/*!
  @brief Processes the given sample and sets item state accordingly.

  @param[i]  item_ptr      - report item
  @param[i]  sample_ptr    - sample from device
  @param[o]  timestamp_ptr - destination for adjusted timestamp
  @param[o]  dest_data     - destination for the resulting data (xyz)

  @return
  SNS_DDF_SUCCESS - sample processed successfully
  SNS_DDF_PENDING - no valid sample was created using given sample
  SNS_DDF_EFAIL - given sample already processed
*/
/*=========================================================================*/
SNS_RH_UIMAGE_CODE sns_ddf_status_e sns_rh_process_sample(
  sns_rh_rpt_item_s*        item_ptr,
  const sns_smgr_sample_s*  sample_ptr,
  sns_ddf_time_t*           timestamp_ptr,
  int32_t                   dest_data[SNS_SMGR_SENSOR_DIMENSION_V01])
{
  sns_ddf_status_e sample_status = SNS_DDF_PENDING;
  sns_rh_interpolator_obj_s* int_obj_ptr = &item_ptr->interpolator_obj;
  sns_smgr_sample_s tmp_sample;
  sns_ddf_time_t sample_timestamp;
  sns_ddf_status_e status;
  bool same_sample;
  /* -------------------------------------------------------------------- */

  if ( timestamp_ptr == NULL || dest_data == NULL || sample_ptr == NULL)
  {
    SNS_SMGR_PRINTF3(
      ERROR, "process_sample - bad param timestamp_ptr=%u dest_data=%u sample_ptr=%u", 
      timestamp_ptr, dest_data, sample_ptr);
    return SNS_DDF_EINVALID_PARAM;
  }
  sample_timestamp = SMGR_SAMPLE_TIMESTAMP(sample_ptr);

  SNS_SMGR_SAMPLE_PROC_DBG3(
    HIGH, "process_sample - rpt_id=%u/0x%x quality=%u",
    item_ptr->parent_report_ptr->rpt_id, 
    item_ptr->parent_report_ptr->msg_header.connection_handle, item_ptr->quality);
  SNS_SMGR_SAMPLE_PROC_DBG4(
    HIGH, "process_sample - ts=%u status=%u last_proc=%u last_sent=%u",
    sample_timestamp, SMGR_SAMPLE_STATUS(sample_ptr), 
    item_ptr->ts_last_sent, item_ptr->last_processed_sample_timestamp);

  if ( item_ptr->last_processed_sample_timestamp != sample_timestamp )
  {
    item_ptr->last_processed_sample_timestamp = sample_timestamp;
    same_sample = false;
  }
  else
  {
    same_sample = true;
  }

  item_ptr->state = RH_RPT_ITEM_STATE_IDLE;
  switch ( item_ptr->quality )
  {
  case SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01:
    if ( !same_sample )
    {
      if ( item_ptr->sampling_count == 0 )
      {
        if ( ((sample_timestamp - item_ptr->ts_last_sent) >= 
              (item_ptr->sampling_interval >> 1)) ||
             (item_ptr->parent_report_ptr->ind_count == 0) ||
              sns_smgr_is_event_sensor(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                       item_ptr->ddf_sensor_ptr->data_type) )
        {
          SNS_OS_MEMCOPY(dest_data, sample_ptr->data, sizeof(sample_ptr->data));
          *timestamp_ptr = sample_timestamp;
          item_ptr->state = RH_RPT_ITEM_STATE_DONE;
          sample_status = SMGR_SAMPLE_STATUS(sample_ptr);
        }
        else /* sample too close to last sample sent */
        {
          SNS_SMGR_PRINTF4(
            MED, "process_sample - rpt_id=%u/0x%x sensor=%u skipped ts=%u", 
            item_ptr->parent_report_ptr->rpt_id,
            item_ptr->parent_report_ptr->msg_header.connection_handle,
            SMGR_SENSOR_TYPE(item_ptr->ddf_sensor_ptr->sensor_ptr,
                             item_ptr->ddf_sensor_ptr->data_type), sample_timestamp);
        }
      }
      item_ptr->sampling_count =
        (item_ptr->sampling_count+1) % item_ptr->sampling_factor;
    }
    else
    {
      sample_status = SNS_DDF_EFAIL;
    }
    break;

  case SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01:
    if ( !same_sample )
    {
      sns_rh_interpolator_update(int_obj_ptr, sample_ptr);
    }
    sample_status = sns_rh_interpolate(int_obj_ptr, dest_data);
    SNS_SMGR_SAMPLE_PROC_DBG4(
      MED, "process_sample - rpt_id=%d desired=%u have=%u status=%d",
      item_ptr->parent_report_ptr->rpt_id, int_obj_ptr->desired_timestamp,
      sample_timestamp, sample_status);
    if ( sample_status != SNS_DDF_EINVALID_PARAM )
    {
      *timestamp_ptr = int_obj_ptr->desired_timestamp;
      item_ptr->state = RH_RPT_ITEM_STATE_DONE;
    }
    else
    {
      sample_status = SNS_DDF_PENDING;
    }
    break;

  case SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01:
    if ( !same_sample )
    {
      sns_rh_interpolator_update(int_obj_ptr, sample_ptr);
      SNS_SMGR_SAMPLE_PROC_DBG3(
        MED, "interp fed - ts1=%u desired=%u ts2=%u",
        int_obj_ptr->sample1_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample1_ptr) : 0,
        int_obj_ptr->desired_timestamp,
        int_obj_ptr->sample2_ptr ? SMGR_SAMPLE_TIMESTAMP(int_obj_ptr->sample2_ptr) : 0);
    }
    SNS_OS_MEMZERO(&tmp_sample, sizeof(tmp_sample));
    status = SNS_DDF_SUCCESS;
    while ( (status == SNS_DDF_SUCCESS) && (sample_status == SNS_DDF_PENDING) )
    {
      status = sns_rh_interpolate(int_obj_ptr, tmp_sample.data);

      SNS_SMGR_SAMPLE_PROC_DBG2(
        LOW, "interpolated - status=%d ts=%u",
        status, int_obj_ptr->desired_timestamp);
      if ( status != SNS_DDF_EINVALID_PARAM )
      {
        SMGR_SAMPLE_TIMESTAMP_SET(&tmp_sample, int_obj_ptr->desired_timestamp);
        SMGR_SAMPLE_STATUS_SET(&tmp_sample, status);
        sample_status = sns_rh_cic_update(&item_ptr->cic_obj, &tmp_sample,
                                          timestamp_ptr, dest_data);
        SNS_SMGR_SAMPLE_PROC_DBG3(
          LOW, "cic - status=%d ts=%u count=%d",
          sample_status, *timestamp_ptr, item_ptr->cic_obj.count);
        if ( sample_status == SNS_DDF_PENDING )
        {
          RH_UPDATE_INTERP_TIMESTAMP(int_obj_ptr);
        }
        else
        {
          item_ptr->state = RH_RPT_ITEM_STATE_DONE;
        }
      }
    }
    break;

  case SNS_SMGR_ITEM_QUALITY_FILTERED_V01:
    if ( same_sample )
    {
      sample_status = SNS_DDF_EFAIL;
    }
    else
    {
      sample_status = sns_rh_cic_update(&item_ptr->cic_obj, sample_ptr,
                                        timestamp_ptr, dest_data);
      if ( sample_status != SNS_DDF_PENDING )
      {
        item_ptr->state = RH_RPT_ITEM_STATE_DONE;
      }
    }
    break;
  }
  SNS_SMGR_SAMPLE_PROC_DBG3(
    LOW, "process_sample - ts=%u status=%u item_state=%u",
    *timestamp_ptr, sample_status, item_ptr->state);
  return sample_status;
}

