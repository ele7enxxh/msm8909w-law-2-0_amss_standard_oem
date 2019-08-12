/*==============================================================================

    S E N S O R S   AKM 8975 M A G N E T O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the magnetometer driver

Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_uimg_mag_akm8975.c#1 $


when         who    what, where, why
----------   ---    ----------------------------------------------------------- 
03/26/2015   ps     Updated license header
03/10/2015   MW     Eliminated KW error
07/20/2014   VY     Disabled logging in uImage
06/11/2014   vy     Refactored for uImage
09/13/12     vh     Eliminated compiler warnings
06/20/12     ag     Removing feature FEATURE_AKM8975_SELF_TEST_IMPL. Assume 
                    it is always defined.
06/19/12     vh     Added run time switch for I2C code
02/02/12     ad     Send error report to sensor manager with valid sensor id 
01/06/12     SD     Changed max frequence from 60hz to 50hz 
11/09/11     YK     Added support for axis mapping. 
10/12/11     AG     Don't release timer when init fails
10/11/11     br     deleted line which releases resources when init fail(we need the resource for get_attr()..)
09/27/11     AG     Moved definition of FEATURE_AKM8975_SELF_TEST_IMPL to arm7.scons
09/21/11     AG     Featurize self test; implement AKM review feedback
09/01/11     AG     Implement changes from code review
08/28/11     AG     Added self test functionality 
06/23/11     dc     removed debug flag for external release
06/04/11     br     Changed to honor SENSOR__ALL too in addition to SENSOR_MAG. Changed log msg level
03/24/11     OF     Adjust sensitivity values to the measurement data read out
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
01/18/11     OF     Split mag driver to AK8975 & AK8973 separte files, splited from sns_dd_mag.c version#8
04/08/10     RY     Added axis conversion for FLUID and FFA/SURF.
==============================================================================*/

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_log_api.h"
#include "sns_log_types.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_dd_mag_akm8975_priv.h"


/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/


/*=======================================================================

                  Extern Variables

========================================================================*/
extern volatile boolean EnableI2C;

/*=======================================================================

  Global Data Definitions

 ========================================================================*/


/*=======================================================================

                   FUNCTION PROTOTYPES

========================================================================*/
extern sns_ddf_status_e sns_dd_akm8975_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

extern sns_ddf_status_e sns_dd_akm8975_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

extern sns_ddf_status_e sns_dd_akm8975_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

extern sns_ddf_status_e sns_dd_akm8975_reset(
        sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_akm8975_self_test(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_test_e       test,
        uint32_t             *err);


/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static void sns_dd_akm8975_handle_timer(
        sns_ddf_handle_t dd_handle,
        void* arg);

static void sns_dd_akm8975_hdle_irq(
  sns_ddf_handle_t dd_handle,
  uint32_t         gpio_num,
  sns_ddf_time_t   timestamp);

static sns_ddf_status_e sns_dd_mgt_akm_start_meas_8975
(
  sns_dd_mag_akm8975_state_s *state
);

static void sns_dd_mag8975_notify_smgr(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_sensor_data_s*      sensor_data_report,
  uint32_t                    num_sensors
);

static void sns_dd_mag8975_notify_smgr_err(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_status_e            err_code,
  sns_ddf_sensor_data_s*      sensor_data_report,
  uint32_t                    num_sensors
);

static void sns_mgt_akm_sample_sensor_8975(
   sns_dd_mag_akm8975_state_s *state
);

static void sns_dd_mag8975_log_data(
   sns_dd_mag_akm8975_state_s *state,
   sns_ddf_time_t sample_time
);

static void sns_mgt_akm_self_test_result(
   sns_dd_mag_akm8975_state_s *state
);


/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_mag_akm8975_driver_fn_list =
{
  &sns_dd_akm8975_init,
  &sns_dd_akm8975_get_data,
  &sns_dd_akm8975_set_attr,
  &sns_dd_akm8975_get_attr,
  &sns_dd_akm8975_handle_timer,
  &sns_dd_akm8975_hdle_irq,
  &sns_dd_akm8975_reset,
  &sns_dd_akm8975_self_test
};


/*###########################################################################*/
/*                    Internal Utilities APIs                                */
/*###########################################################################*/


/*===========================================================================

  FUNCTION:   snsd_mgt_akm_start_meas_8975

===========================================================================*/
/*!
  @brief Program the meas mode on AKM 8975

  @detail
  - Prog the AKM_MAG_REG_ADDR_CNTL_8975 register for meas mode

  @param[in] *state - ptr to the driver state

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mgt_akm_start_meas_8975
(
  sns_dd_mag_akm8975_state_s *state
)
{
  uint8_t           akm_write_buffer, out;
  sns_ddf_status_e  err_code = SNS_DDF_SUCCESS;

  /* start measurement mode */
  akm_write_buffer = AKM_MAGNETOMETER_SINGLE_MEAS_MODE_8975;
  if((err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_CNTL_8975,
                          &akm_write_buffer,
                          DDF_WRITE_ONE_BYTE,
                          &out)) == SNS_DDF_SUCCESS)
  {
    err_code = sns_ddf_timer_start(state->sns_dd_mag_tmr_obj, USEC_TIME_FOR_MEASUREMENT_8975);
  }
  return err_code;
}

/*===========================================================================
  FUNCTION:   sns_dd_mag8975_log_data
===========================================================================*/
/*!
  @brief log sensor data for AKM8975 magnetometer

  @detail
  - Logs latest set of sensor data sampled from the AKM8975 mag sensor.

  @param[in] state: ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
  None

*/
/*=========================================================================*/
static void sns_dd_mag8975_log_data(sns_dd_mag_akm8975_state_s *state,
                         sns_ddf_time_t sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  if( sns_ddf_smgr_is_in_uimage() )
    return;
    
  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_MAG;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AKM;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = AKM_MAG8975_NUM_DATATYPES;
    log_struct_ptr->data[0]        = state->data_cache[0];
    log_struct_ptr->data[1]        = state->data_cache[1];
    log_struct_ptr->data[2]        = state->data_cache[2];


    //Commit log (also frees up the log packet memory)
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log_struct_ptr);
  }
}


/*===========================================================================

  FUNCTION:   sns_dd_mag8975_notify_smgr

===========================================================================*/
/*!
  @brief Called SMGR needs to be notified of data ready

  @note This will routine will be called from
  sns_mgt_akm_sample_sensor_* routine when new data is ready

  @param[in] *state               ptr to the driver state structure
  @param[in] sensor_data_report   Sensor data to pass to the Sensors Manager.
  @param[in] num_sensors          Length of sensor_data_report.

  @return
  None

*/
/*=========================================================================*/
static void sns_dd_mag8975_notify_smgr(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_sensor_data_s*      sensor_data_report,
  uint32_t                    num_sensors)
{
#ifndef MAG8975_UNIT_TEST
  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, num_sensors);
#endif
}

/*===========================================================================

  FUNCTION:   sns_dd_mag8975_notify_smgr_err

===========================================================================*/
/*!
  @brief Called SMGR needs to be notified of error

  @note This will routine will be called from
  sns_mgt_akm_sample_sensor_* routine when an error is seen in memory
  allocation for sample data or problem in reading the data from the
  Mag. The top level sensor data structure with filled with appropriate
  error type before calling the sns_dd_mag8975_notify_smgr routine

  @param[in] *state               ptr to the driver state structure
  @param[in] err_code             error code for the problem
  @param[in] sensor_data_report   Sensor data to pass to the Sensors Manager.
  @param[in] num_sensors          Length of sensor_data_report.

  @return
  None

  $TODO:
*/
/*=========================================================================*/
static void sns_dd_mag8975_notify_smgr_err(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_status_e            err_code,
  sns_ddf_sensor_data_s*      sensor_data_report,
  uint32_t                    num_sensors)
{
  uint8_t i;

  for(i=0; i<(uint8_t)num_sensors; i++)
  {
    sensor_data_report[i].sensor = SNS_DDF_SENSOR_MAG; 
    sensor_data_report[i].status = err_code;
    sensor_data_report[i].timestamp = sns_ddf_get_timestamp();
  }

  sns_dd_mag8975_notify_smgr(state, sensor_data_report, num_sensors);
}

/*===========================================================================

  FUNCTION:   sns_mgt_akm_sample_sensor_8975

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired.

  @note This routine will read the data status & read the data and copy
  it at a malloced location. The address of the data location is passed
  as parameter in the notify_data routine.

  @param[in] *state   ptr to the driver state structure

  @return
  None

  $TODO:
*/
/*=========================================================================*/
static void sns_mgt_akm_sample_sensor_8975(sns_dd_mag_akm8975_state_s *state)
{
  uint8_t                   akm_read_data[MAG_DATA_NUM_BYTES]= {0}, out, size;
  int16_t                   temp_q16[AKM_MAG8975_NUM_DATATYPES];
  sns_ddf_status_e          err_code;
  sns_ddf_sensor_data_s*    sensor_data_report;
  sns_ddf_sensor_sample_s*  data_ptr;
  int                       i;

  /* Resetting the state to idle once into this routine implying
  whatever the error case SMGR could request for data again & the driver
  would come back with appropriate error condition again instead of
  nacking based on state being not idle*/
  state->curr_state = SNSD_MGT8975_ST_IDLE;

  size = sizeof(sns_ddf_sensor_data_s);
  if((err_code = sns_ddf_malloc((void **)&sensor_data_report, size)) != SNS_DDF_SUCCESS)
  {
    return;
  }

  /* Read ST1, 6 data registers, ST2 in a single 8-byte I2C transaction.
     Then check ST1 and ST2 values to determine if data is really good */
  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAG_REG_ADDR_STATUS1_8975,
                              akm_read_data,
                              MAG_DATA_NUM_BYTES,
                              &out)) != SNS_DDF_SUCCESS)
  {
    //Handling the err cond when status from Mag is not a success
    sns_dd_mag8975_notify_smgr_err(state, err_code, sensor_data_report, 1);
    sns_ddf_mfree(sensor_data_report);

    return;
  }

  if (EnableI2C == TRUE)
  {
    if((akm_read_data[0] & AKM_MAGNETOMETER_DRDY_BIT_MASK) == 0)
    {
      /* Handling the err cond when status indicates data is still not
               ready after giving it ample time to be ready 7msec */
      sns_dd_mag8975_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, 1);
      sns_ddf_mfree(sensor_data_report);

      return;
    }

    if(akm_read_data[7] & AKM_MAGNETOMETER_HOFL_BIT_MASK)
    {
      /* Handling the err cond when magnetic sensor may overflow even though measurement
               data regiseter is not saturated */
      sns_dd_mag8975_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, 1);
      sns_ddf_mfree(sensor_data_report);
      return;
    }
  }

//Storing raw data in 16 bits
  temp_q16[0] = akm_read_data[1]|(akm_read_data[2]<<8);
  temp_q16[1] = akm_read_data[3]|(akm_read_data[4]<<8);
  temp_q16[2] = akm_read_data[5]|(akm_read_data[6]<<8);

  for(i=0 ; i<3 ; i++)
  {
    // Adjust sensitivity values to the measurement data read out from the measurement data register
    temp_q16[i] = temp_q16[i]*state->sens_adj[i];
    /* convert based on base range */
    state->data_cache[i] = AKM8975_RAW_TO_GAUSS(temp_q16[i]);
  }

  size = AKM_MAG8975_NUM_DATATYPES*sizeof(sns_ddf_sensor_sample_s);
  if((err_code = sns_ddf_malloc((void **)&data_ptr, size)) != SNS_DDF_SUCCESS)
  {
    // Handling the err cond when no mem avail to give data samples to SMGR
    sns_dd_mag8975_notify_smgr_err(state, SNS_DDF_ENOMEM, sensor_data_report, 1);
    sns_ddf_mfree(sensor_data_report);
    return;
  }

  // Convert sensor data to the phone's coordinate system.
  sns_ddf_map_axes(&state->axes_map, state->data_cache);

  if (EnableI2C == TRUE)
  {
    data_ptr[0].sample = state->data_cache[0];
    data_ptr[1].sample = state->data_cache[1];
    data_ptr[2].sample = state->data_cache[2];
  }
  else
  {
    static uint32 magDummyData = 2001;
    data_ptr[0].sample = magDummyData++;
    data_ptr[1].sample = magDummyData++;
    data_ptr[2].sample = magDummyData++;
  }

  data_ptr[0].status = data_ptr[1].status = data_ptr[2].status = SNS_DDF_SUCCESS;

  MAG8975_DBG_MSG_3(DBG_DD_MAG8975_GET_DATA_REQ, state->data_cache[0], state->data_cache[1], state->data_cache[2]);

  /* mark data as ready */
  sensor_data_report->sensor      = SNS_DDF_SENSOR_MAG;
  sensor_data_report->status      = SNS_DDF_SUCCESS;
  sensor_data_report->timestamp   = sns_ddf_get_timestamp();
  sensor_data_report->samples     = data_ptr;
  sensor_data_report->num_samples = AKM_MAG8975_NUM_DATATYPES;

  sns_dd_mag8975_notify_smgr(state, sensor_data_report, 1);
  sns_dd_mag8975_log_data(state, sensor_data_report->timestamp);

  sns_ddf_mfree(data_ptr);
  sns_ddf_mfree(sensor_data_report);
}


/*===========================================================================

  FUNCTION:   sns_mgt_akm_self_test_result

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. 
 
  @note This routine will read the data status & read the data and assess if
  data is within range to determine pass/fail verdict.
 
  @param[in] *state   ptr to the driver state structure

  @return
  None
 
  $TODO: 
*/
/*=========================================================================*/
static void sns_mgt_akm_self_test_result(sns_dd_mag_akm8975_state_s *state)
{
  uint8_t                   akm_read_data[MAG_DATA_NUM_BYTES], out;
  int                       i;
  uint8_t					akm_write_buffer_astc;
  int16_t                   data[AKM_MAG8975_NUM_DATATYPES];

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8400);

  /* Reset the state to idle */
  state->curr_state = SNSD_MGT8975_ST_IDLE;

  /* Sensor transitions to low power automatically */
  state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;

  /* Read ST1, 6 data registers, ST2 in a single 8-byte I2C transaction. 
     Then check ST1 and ST2 values to determine if data is really good */
  if ( sns_ddf_read_port(state->port_handle,
                         AKM_MAG_REG_ADDR_STATUS1_8975, 
                         akm_read_data,
                         MAG_DATA_NUM_BYTES,
                         &out) != SNS_DDF_SUCCESS)
  {
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_I2C);
    return;
  }

  akm_write_buffer_astc = AKM_MAGNETOMETER_ASTC_SELF_TEST_DISABLE;
  if ( sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_SELF_TEST_8975,
                          &akm_write_buffer_astc,
                          DDF_WRITE_ONE_BYTE,  
                          &out) != SNS_DDF_SUCCESS)
  {
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_I2C);
    return;
  }

  if((akm_read_data[0] & AKM_MAGNETOMETER_DRDY_BIT_MASK) == 0)
  {
    /* Handling the err cond when status indicates data is still not
       ready after giving it ample time to be ready 7msec */
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_DRDY);
    return;
  }

  if (akm_read_data[7] & AKM_MAGNETOMETER_HOFL_BIT_MASK)
  {
    /* Handling the err cond when magnetic sensor may overflow even though measurement 
      data regiseter is not saturated */
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_OVFL);
    return;
  }

  /* raw data in 16 bits  */
  data[0] = akm_read_data[1]|(akm_read_data[2]<<8);
  data[1] = akm_read_data[3]|(akm_read_data[4]<<8);
  data[2] = akm_read_data[5]|(akm_read_data[6]<<8);  

  MAG8975_SMGR_DBG_MSG_3(DBG_SMGR_GENERIC_STRING3,
                         data[0],
                         data[1],
                         data[2]);

  MAG8975_SMGR_DBG_MSG_3(DBG_SMGR_GENERIC_STRING3, 
                         state->sens_cache[0], state->sens_cache[1], 
                         state->sens_cache[2]);

  for(i=0 ; i<3 ; i++)
  {
    /* Adjust sensitivity values to the measurement data read
       out from the measurement data register */
    state->data_cache[i] = (data[i]*state->sens_adj[i]);
  }

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8500);

  MAG8975_SMGR_DBG_MSG_3(DBG_SMGR_GENERIC_STRING3,
                         state->data_cache[0],
                         state->data_cache[1],
                         state->data_cache[2]);

  /* check if data is within valid range */
  if ((FX_ABS(state->data_cache[0]) <= 100.0) &&
      (FX_ABS(state->data_cache[1]) <= 100.0) &&
      ((state->data_cache[2] >= -1000.0) && 
       (state->data_cache[2] <= -300.0)))
  {
    MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8600);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_SUCCESS,
                                      SNSD_MAG_ERR_GENERIC);
  }
  else
  {
    MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8700);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_RESULT_OUT_OF_RANGE);
  }

  sns_dd_mag8975_log_data(state, sns_ddf_get_timestamp()); 

}


/*===========================================================================

  FUNCTION:   sns_dd_akm8975_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This must be
         the callback function submitted when initializing a timer.

  @note This will be called within the context of the Sensors Manager task.

     @param[in] dd_handle   Handle to a driver instance.
     @param[in] arg         The argument submitted when the timer was set.

  @return
    None

  $TODO:
*/
/*=========================================================================*/
void sns_dd_akm8975_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  sns_dd_mag_akm8975_state_s *state = (sns_dd_mag_akm8975_state_s *)dd_handle;

  if (state->curr_state == SNSD_MGT8975_ST_TEST_STARTED)
  {
    sns_mgt_akm_self_test_result(state);
  }
  else
  {
    sns_mgt_akm_sample_sensor_8975(state);
  }
}

/*###########################################################################
  *                    DDF APIs
 ###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors. Mag is programmed to measurement mode where it would sample
    data and a few msec later be ready with the three axis Mag data.
    Hence a timer is set to be called back with the appropriate delay
    which would be the time to data ready state

  @param[in] dd_handle     Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm8975_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[])
{
  sns_ddf_status_e err_code;
  sns_dd_mag_akm8975_state_s *state = (sns_dd_mag_akm8975_state_s *)dd_handle;

  /* state machine */
  switch (state->curr_state)
  {
    case SNSD_MGT8975_ST_IDLE:

      if((err_code = sns_dd_mgt_akm_start_meas_8975(state)) == SNS_DDF_SUCCESS)
      {
        state->curr_state = SNSD_MGT8975_ST_STARTED;
        /* Mag returns data asynchronously. Hence set PENDING
               * status to return to Sensor Manager */
        err_code = SNS_DDF_PENDING;

        #ifdef MAG8975_UNIT_TEST
        sns_ddf_delay(USEC_TIME_FOR_MEASUREMENT_8975);
        sns_dd_akm8975_handle_timer(dd_handle, NULL);
        err_code = SNS_DDF_SUCCESS;
        #endif

      }
    break;

    case SNSD_MGT8975_ST_STARTED:
      /* This is added for completeness and making SMGR aware that it first
      needs to change the HW device state to active and then get data which
      in case of mag doesn't actually make a difference*/
      err_code = SNS_DDF_PENDING;
      MAG8975_DBG_MSG_1(DBG_DD_MAG8975_GET_ERROR, state->curr_state);
    break;

    case SNSD_MGT8975_ST_TEST_STARTED:
      /* If self test is currently ongoing, streaming cannot be supported */ 
      err_code = SNS_DDF_EDEVICE_BUSY;
      MAG8975_DBG_MSG_1(DBG_DD_MAG8975_GET_ERROR, state->curr_state);
    break;

    default:
      err_code = SNS_DDF_EFAIL;
    break;
  }

  return err_code;
}


/*===========================================================================

  FUNCTION:   sns_dd_akm8975_hdle_irq

===========================================================================*/
/*!
  @brief Called by the SMGR in response to an interrupt

  @detail

    @param[in] dd_handle  Handle to a driver instance.
    @param[in] irq        The IRQ representing the interrupt that occured.

  @return
  None

  $TODO:
*/
/*=========================================================================*/
void sns_dd_akm8975_hdle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp)
{
  MAG8975_DBG_MSG_0(DBG_DD_MAG8975_HANDLE_IRQ_REQ);
  return;
}




