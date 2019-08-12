/*==============================================================================

    S E N S O R S   AKM 099XX M A G N E T O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the magnetometer driver

-- Copyright Notice --

  Copyright (c) 2004 Asahi Kasei Microdevices Corporation, Japan
  All Rights Reserved.

-- End Asahi Kasei Microdevices Copyright Notice --
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm099xx.c#1 $


when         who    what, where, why
----------   ---    -----------------------------------------------------------
03/10/15     MW     Eliminate KW error
08/16/13     AKM    Combile AK09912/AK09911.
02/28/13     AKM    AK09912
09/13/12     vh     Eliminated compiler warnings
08/23/12     ag     Fix crash on 8974 target
02/02/12     ad     Send error report to sensor manager with valid sensor id
01/06/12     SD     changed max rate from 60hz to 50hz
12/12/11     SD     added sns_ddf_axes_map_init
10/12/11     AG     Don't release timer when init fails
10/12/11     RY     Inverted x-axis polarity
10/11/11     br     deleted line which releases resources when init fail(we need the resource for get_attr()..)
10/06/11     RY     Updated polarity for FLUID
09/15/11     RY     Modified for AK8963
09/01/11     AG     Implement changes from code review
08/28/11     AG     Added self test functionality
06/23/11     dc     removed debug flag for external release
06/04/11     br     Changed to honor SENSOR__ALL too in addition to SENSOR_MAG. Changed log msg level
03/24/11     OF     Adjust sensitivity values to the measurement data read out
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
01/18/11     OF     Split mag driver to AK8963 & AK8973 separte files, splited from sns_dd_mag.c version#8
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
#include "sns_dd_mag_akm099xx_priv.h"
#include "sns_dd_mag_akm099xx_tlim.h"


/*==============================================================================

  Preprocessor Definitions and Constants

==============================================================================*/

/* Temporary, until we will have release will all DDF changes available*/
//#define USE_CC41

/*
 * Convert from AKM unit to QTI unit.
 */
#define UTESLA_TO_GAUSS         (0.01f)   /* Convert from uT to Gauss */

/* 8.5 msec maximum time is enough for both AK09912 and AK09911.
 * section 5.3.3 in AK09912 data sheet version MS1547_E-00
 * section 5.3.3 in AK09911 data sheet version MS1526_E-00
 */
#define USEC_TIME_FOR_MEASUREMENT 8500
#define NUM_MEASUREMENT_RETRY     5


/* Should be commented when checked-in */
//#define MAG_AKM_DEBUG

#ifdef MAG_AKM_DEBUG
#define MAG_AKM_DBG_MSG_0( msg ) \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_DD_MAG8975, msg )
#define MAG_AKM_DBG_MSG_1( msg, p1 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_DD_MAG8975, msg, p1 )
#define MAG_AKM_DBG_MSG_2( msg, p1, p2 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_DD_MAG8975, msg, p1, p2 )
#define MAG_AKM_DBG_MSG_3( msg, p1, p2, p3 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_DD_MAG8975, msg, p1, p2, p3 )
#else
#define MAG_AKM_DBG_MSG_0( msg )
#define MAG_AKM_DBG_MSG_1( msg, p1 )
#define MAG_AKM_DBG_MSG_2( msg, p1, p2 )
#define MAG_AKM_DBG_MSG_3( msg, p1, p2, p3 )
#endif

//#define MAG_AKM_SELF_TEST_DEBUG

#ifdef MAG_AKM_SELF_TEST_DEBUG
#define MAG_AKM_SMGR_DBG_MSG_0( msg ) \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_SMGR, msg )
#define MAG_AKM_SMGR_DBG_MSG_1( msg, p1 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_SMGR, msg, p1 )
#define MAG_AKM_SMGR_DBG_MSG_2( msg, p1, p2 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_SMGR, msg, p1, p2 )
#define MAG_AKM_SMGR_DBG_MSG_3( msg, p1, p2, p3 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_SMGR, msg, p1, p2, p3 )
#else
#define MAG_AKM_SMGR_DBG_MSG_0( msg )
#define MAG_AKM_SMGR_DBG_MSG_1( msg, p1 )
#define MAG_AKM_SMGR_DBG_MSG_2( msg, p1, p2 )
#define MAG_AKM_SMGR_DBG_MSG_3( msg, p1, p2, p3 )
#endif

/* Use define to avoid numbers in code */
#define DDF_WRITE_ONE_BYTE       1
#define DDF_READ_ONE_BYTE        1
#define DDF_WRITE_TWO_BYTES      2
#define DDF_READ_TWO_BYTES       2

extern volatile boolean EnableI2C;

/*==============================================================================

  Global Data Definitions

==============================================================================*/
/*static*/ sns_dd_mag_akm099xx_state_s *drv_akm099xx_dbg = NULL;

static sns_ddf_sensor_e sns_dd_mag_sensor_types[] =
{
  SNS_DDF_SENSOR_MAG
};

/*==============================================================================

  INTERNAL FUNCTION PROTOTYPES

==============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_akm099xx_get_data(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensors[],
    uint32_t                 num_sensors,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_data_s*   data[]);

static sns_ddf_status_e sns_dd_akm099xx_set_attr(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensor,
    sns_ddf_attribute_e      attrib,
    void*                    value);

static sns_ddf_status_e sns_dd_akm099xx_get_attr(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensor,
    sns_ddf_attribute_e      attrib,
    sns_ddf_memhandler_s*    memhandler,
    void**                   value,
    uint32_t*                num_elems);

static void sns_dd_akm099xx_handle_timer(
    sns_ddf_handle_t         dd_handle,
    void*                    arg);

static void sns_dd_akm099xx_hdle_irq(
    sns_ddf_handle_t         dd_handle,
    uint32_t                 gpio_num,
    sns_ddf_time_t           timestamp);

static sns_ddf_status_e sns_dd_akm099xx_reset(
    sns_ddf_handle_t         dd_handle);

static sns_ddf_status_e sns_dd_akm099xx_self_test(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensor,
    sns_ddf_test_e           test,
    uint32_t*                err);

static sns_ddf_status_e sns_dd_akm099xx_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors);

/*==============================================================================

  STRUCTURE DEFINITIONS

==============================================================================*/
sns_ddf_driver_if_s sns_mag_akm_driver_fn_list =
{
#ifdef WIN32
  &sns_dd_akm099xx_init,
  &sns_dd_akm099xx_get_data,
  &sns_dd_akm099xx_set_attr,
  &sns_dd_akm099xx_get_attr,
  &sns_dd_akm099xx_handle_timer,
  &sns_dd_akm099xx_hdle_irq,
  &sns_dd_akm099xx_reset,
  &sns_dd_akm099xx_self_test,
  NULL,
  &sns_dd_akm099xx_probe
#else
  .init              = &sns_dd_akm099xx_init,
  .get_data          = &sns_dd_akm099xx_get_data,
  .set_attrib        = &sns_dd_akm099xx_set_attr,
  .get_attrib        = &sns_dd_akm099xx_get_attr,
  .handle_timer      = &sns_dd_akm099xx_handle_timer,
  .handle_irq        = &sns_dd_akm099xx_hdle_irq,
  .reset             = &sns_dd_akm099xx_reset,
  .run_test          = &sns_dd_akm099xx_self_test,
  .enable_sched_data = NULL,
  .probe             = &sns_dd_akm099xx_probe
#endif
};

/*###########################################################################*/
/*                    Internal Utilities APIs                                */
/*###########################################################################*/

/*==============================================================================

  FUNCTION:   sns_mgt_akm_set_operation_mode_099xx

==============================================================================*/
/*!
  @brief Program the meas mode on AKM099XX

  @detail
  - Prog the AKM_MAG_REG_ADDR_CNTL2_099XX register for meas mode

  @param[in] *state - ptr to the driver state
  @param[in] mode   - next mode

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;
   Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
   SNS_DDF_EFAIL to indicate an error has occurred.

  $TODO:
  */
/*============================================================================*/
static sns_ddf_status_e sns_mgt_akm_set_operation_mode_099xx(
    sns_dd_mag_akm099xx_state_s *state,
    mag099xx_operation_mode_e   mode)
{
  uint16_t          write_address;
  uint8_t           write_buffer[2], write_num, out;
  bool              trigger_timer;
  sns_ddf_status_e  err_code = SNS_DDF_SUCCESS;

  /* start measurement mode */
  switch(mode)
  {
    case AKM_MAG_SINGLE_MEAS_MODE_099XX:
      /* CNTL1 register value is different */
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        write_address   = AKM_MAG_REG_ADDR_CNTL1_099XX;
        write_buffer[0] = AKM_MAG09912_NSF_BIT_MASK;
        write_buffer[1] = mode;
        write_num       = DDF_WRITE_TWO_BYTES;
        trigger_timer   = true;
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        write_address   = AKM_MAG_REG_ADDR_CNTL2_099XX;
        write_buffer[0] = mode;
        write_num       = DDF_WRITE_ONE_BYTE;
        trigger_timer   = true;
      }
      else
      {
        return SNS_DDF_EDEVICE;
      }
      break;

    case AKM_MAG_SELF_TEST_MODE_099XX:
      write_address   = AKM_MAG_REG_ADDR_CNTL2_099XX;
      write_buffer[0] = mode;
      write_num       = DDF_WRITE_ONE_BYTE;
      trigger_timer   = true;
      break;

    case AKM_MAG_PWR_DOWN_MODE_099XX:
    case AKM_MAG_FUSE_ROM_ACCESS_MODE_099XX:
      write_address   = AKM_MAG_REG_ADDR_CNTL2_099XX;
      write_buffer[0] = mode;
      write_num       = DDF_WRITE_ONE_BYTE;
      trigger_timer   = false;
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  if ((err_code = sns_ddf_write_port(
          state->port_handle,
          write_address,
          write_buffer,
          write_num,
          &out)) != SNS_DDF_SUCCESS)
  {
    return err_code;
  }

  if (trigger_timer)
  {
    /* Reset retry number first */
    state->retry = 0;
    /* Start timer */
    if ((err_code = sns_ddf_timer_start(
            state->sns_dd_mag_tmr_obj,
            USEC_TIME_FOR_MEASUREMENT)) != SNS_DDF_SUCCESS)
    {
      return err_code;
    }
  }

  /* 100 usec delay is needed after set operation mode */
  sns_ddf_delay(100);

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_config_pwr_mode

==============================================================================*/
/*!
  @brief Routine used to set the power state when set as an attrib by
  SMGR that calls the appropriate pwr setting routines based on the
  device type requested

  @detail
  When power is turned ON, AK099XX is in power-down mode. When any other mode
  is selected the AK099XX transits to the specified mode and starts operation.
  When operation finishes the AK099XX turns back to power-down mode.
  Therefore there isn't much difference between SNS_DDF_POWERSTATE_LOWPOWER and
  SNS_DDF_POWERSTATE_ACTIVE modes. When the former is set, we need to cancel
  any measurement in progress.

  @param[in/out] state: Ptr to the driver structure
  @param[in] *mode: Power state change requested with an pwr attrib set
  routine

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;

  $TODO:
*/
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_config_pwr_mode(
    sns_dd_mag_akm099xx_state_s* state,
    sns_ddf_powerstate_e         mode)
{
  if (mode == state->curr_pwr_state)
  {
    return SNS_DDF_SUCCESS;
  }

  if (mode == SNS_DDF_POWERSTATE_LOWPOWER)
  {
    if (state->curr_state != SNSD_MAG_ST_IDLE)
    {
      if (sns_mgt_akm_set_operation_mode_099xx(
            state,
            AKM_MAG_PWR_DOWN_MODE_099XX) != SNS_DDF_SUCCESS)
      {
        return (SNS_DDF_EDEVICE);
      }
      /* If current state is not IDLE, it means that we start
       * measurement mode and also started the timer */
      sns_ddf_timer_cancel(state->sns_dd_mag_tmr_obj);
      state->curr_state = SNSD_MAG_ST_IDLE;
    }
    state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  }
  else if (mode == SNS_DDF_POWERSTATE_ACTIVE)
  {
    state->curr_pwr_state = SNS_DDF_POWERSTATE_ACTIVE;
  }
  else
  {
    return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

  FUNCTION:   sns_mgt_akm_get_sensitivity_data_099xx

==============================================================================*/
/*!
  @brief Gets the sensitivity adjusment values of the AKM099XX magnetometer

  @detail
  - Get the axis sensitivity adjusment values from the fuse ROM, store it in
    the driver state structure in Q16 gauss units.

  @param[in] *state - ptr to the driver state

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;

  $TODO:
  Write Error string
*/
/*============================================================================*/
static sns_ddf_status_e sns_mgt_akm_get_sensitivity_data_099xx(
    sns_dd_mag_akm099xx_state_s *state)
{
  uint8_t           out;
  uint8_t           read_buffer[AKM_MAG099XX_NUM_SENSITIVITY];
  sns_ddf_status_e  err_code;
  int               i;

  /* Set Fuse ROM access mode */
  if ((err_code = sns_mgt_akm_set_operation_mode_099xx(
          state,
          AKM_MAG_FUSE_ROM_ACCESS_MODE_099XX)) != SNS_DDF_SUCCESS)
  {
    return err_code;
  }

  /* Read the data */
  if ((err_code = sns_ddf_read_port(
          state->port_handle,
          AKM_MAG_REG_ADDR_X_SEN_ADJUST_099XX,
          read_buffer,
          AKM_MAG099XX_NUM_SENSITIVITY,
          &out)) != SNS_DDF_SUCCESS)
  {
    return err_code;
  }

  for (i = 0; i < AKM_MAG099XX_NUM_SENSITIVITY; ++i)
  {
    if (state->device_select == AKM_MAG09912_DEVICE_ID)
    {
      /* Sensitivity adjusment is calculated acording to
       * AKM09912 datasheet version MS1547-E-00 section 8.3.13 */
      state->sens_adj[i] = (read_buffer[i] / 256.0f) + 0.5f;
    }
    else if (state->device_select == AKM_MAG09911_DEVICE_ID)
    {
      /* Sensitivity adjusment is calculated acording to
       * AKM09911 datasheet version MS1526-E-00 section 8.3.11 */
      state->sens_adj[i] = (read_buffer[i] / 128.0f) + 1.0f;
    }
    else
    {
      return SNS_DDF_EDEVICE;
    }
  }

  /* Set Power-down mode */
  if ((err_code = sns_mgt_akm_set_operation_mode_099xx(
          state,
          AKM_MAG_PWR_DOWN_MODE_099XX)) != SNS_DDF_SUCCESS)
  {
    return err_code;
  }

  MAG_AKM_DBG_MSG_3(
      DBG_DD_MAG8975_SENSITIVITY_DATA,
      read_buffer[0],
      read_buffer[1],
      read_buffer[2]);

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

  FUNCTION:   sns_dd_mag099xx_log_data

==============================================================================*/
/*!
  @brief log sensor data for AKM099XX magnetometer

  @detail
  - Logs latest set of sensor data sampled from the AKM099XX mag sensor.

  @param[in] state: ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
   None

*/
/*============================================================================*/
void sns_dd_mag099xx_log_data(
    sns_dd_mag_akm099xx_state_s *state,
    sns_ddf_time_t              sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  /* Allocate log packet */
  err_code = sns_logpkt_malloc(
      SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s),
      (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_MAG;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AKM;

    /* Timestamp the log with sample time */
    log_struct_ptr->timestamp = sample_time;

    /* Log the sensor data */
    log_struct_ptr->num_data_types = AKM_MAG099XX_NUM_DATATYPES;
    log_struct_ptr->data[0]   = state->data_cache[0];
    log_struct_ptr->data[1]   = state->data_cache[1];
    log_struct_ptr->data[2]   = state->data_cache[2];

    /* Commit log (also frees up the log packet memory) */
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log_struct_ptr);
  }
  return;
}

/*==============================================================================

  FUNCTION:   sns_dd_mag099xx_notify_smgr

==============================================================================*/
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
/*============================================================================*/
static void sns_dd_mag099xx_notify_smgr(
    sns_dd_mag_akm099xx_state_s* state,
    sns_ddf_sensor_data_s*       sensor_data_report,
    uint32_t                     num_sensors)
{
#ifndef MAG_AKM_UNIT_TEST
  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, num_sensors);
#endif
  return;
}

/*==============================================================================

  FUNCTION:   sns_dd_mag099xx_notify_smgr_err

==============================================================================*/
/*!
  @brief Called SMGR needs to be notified of error

  @note This will routine will be called from
  sns_mgt_akm_sample_sensor_* routine when an error is seen in memory
  allocation for sample data or problem in reading the data from the
  Mag. The top level sensor data structure with filled with appropriate
  error type before calling the sns_dd_mag099xx_notify_smgr routine

  @param[in] *state               ptr to the driver state structure
  @param[in] err_code             error code for the problem
  @param[in] sensor_data_report   Sensor data to pass to the Sensors Manager.
  @param[in] num_sensors          Length of sensor_data_report.

  @return
   None

  $TODO:
*/
/*============================================================================*/
static void sns_dd_mag099xx_notify_smgr_err(
    sns_dd_mag_akm099xx_state_s* state,
    sns_ddf_status_e            err_code,
    sns_ddf_sensor_data_s*      sensor_data_report,
    uint32_t                    num_sensors)
{
  uint8_t i;

  for (i = 0; i < (uint8_t)num_sensors; i++)
  {
    sensor_data_report[i].sensor    = SNS_DDF_SENSOR_MAG;
    sensor_data_report[i].status    = err_code;
    sensor_data_report[i].timestamp = sns_ddf_get_timestamp();
  }

  sns_dd_mag099xx_notify_smgr(state, sensor_data_report, num_sensors);
}

/*==============================================================================

  FUNCTION:   sns_mgt_akm_sample_sensor_099xx

==============================================================================*/
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
/*============================================================================*/
static void sns_mgt_akm_sample_sensor_099xx(
    sns_dd_mag_akm099xx_state_s *state)
{
  uint8_t                   akm_read_data[AKM_MAG099XX_NUM_DATA_BYTES];
  uint8_t                   out;
  int16_t                   temp_i16;
  float                     temp_flt;
  sns_ddf_status_e          err_code;
  sns_ddf_sensor_data_s*    sensor_data_report;
  sns_ddf_sensor_sample_s*  data_ptr;
  int                       i;

  if ((err_code = sns_ddf_malloc(
          (void **)&sensor_data_report,
          sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS)
  {
    /* Reset current state */
    state->curr_state = SNSD_MAG_ST_IDLE;
    return;
  }

  /* Read ST1 first. Then check ST1 whether measurement is done or not */
  if ((err_code = sns_ddf_read_port(
          state->port_handle,
          AKM_MAG_REG_ADDR_STATUS1_099XX,
          akm_read_data,
          DDF_READ_ONE_BYTE,
          &out)) != SNS_DDF_SUCCESS)
  {
    /* Handling the err cond when reading operation is failed. */
    sns_dd_mag099xx_notify_smgr_err(state, err_code, sensor_data_report, 1);
    goto COMPLETE_SAMPLE_SENSOR;
  }

  /* Check ST1 bit */
  if ((akm_read_data[0] & AKM_MAG_DRDY_BIT_MASK) == 0)
  {
    /* Measurement is not done yet. */
    if (NUM_MEASUREMENT_RETRY <= ++(state->retry))
    {
      /* When retry number exceeds the limit, send error message. */
      sns_dd_mag099xx_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, 1);
      goto COMPLETE_SAMPLE_SENSOR;
    }

    /* Start timer again */
    if ((err_code = sns_ddf_timer_start(
            state->sns_dd_mag_tmr_obj,
            USEC_TIME_FOR_MEASUREMENT)) != SNS_DDF_SUCCESS)
    {
      /* Timer cannot be started, exit with error */
      sns_dd_mag099xx_notify_smgr_err(state, err_code, sensor_data_report, 1);
      goto COMPLETE_SAMPLE_SENSOR;
    }
    /* This label will not change current status. Just exit this function */
    goto FINALIZE_SAMPLE_SENSOR;
  }

  /* Read ST1, 7 data registers, ST2 in a single 9-byte I2C transaction.
     Then check ST1 and ST2 values to determine if data is really good */
  if ((err_code = sns_ddf_read_port(
          state->port_handle,
          AKM_MAG_REG_ADDR_STATUS1_099XX,
          akm_read_data,
          AKM_MAG099XX_NUM_DATA_BYTES,
          &out)) != SNS_DDF_SUCCESS)
  {
    /* Handling the err cond when reading operation is failed. */
    sns_dd_mag099xx_notify_smgr_err(state, err_code, sensor_data_report, 1);
    goto COMPLETE_SAMPLE_SENSOR;
  }

  /* Check overflow */
  if (akm_read_data[AKM_MAG099XX_NUM_DATA_BYTES - 1] & AKM_MAG_HOFL_BIT_MASK)
  {
    /* Handling the err cond when magnetic sensor may overflow even though
     * measurement data regiseter is not saturated */
    sns_dd_mag099xx_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, 1);
    goto COMPLETE_SAMPLE_SENSOR;
  }

  for (i = 0; i < AKM_MAG099XX_NUM_DATATYPES; i++)
  {
    /* Storing raw data in 16 bits */
    temp_i16 = (int16_t)(((uint16_t)akm_read_data[(i*2)+1]) |
        ((uint16_t)akm_read_data[(i*2)+2]<<8));
    /* Adjust sensitivity values to the measurement data read out from the
     * measurement data register. As a result, 'temp_flt' is unit of Gauss. */
    temp_flt = temp_i16 * state->sens_adj[i] * state->mag_scale_flt;
    /* convert float to Q16 format */
    state->data_cache[i] = FX_FLTTOFIX_Q16(temp_flt);
  }

  /* Convert sensor data to the phone's coordinate system. */
  sns_ddf_map_axes(&state->axes_map, state->data_cache);

  if ((err_code = sns_ddf_malloc(
          (void **)&data_ptr,
          AKM_MAG099XX_NUM_DATATYPES * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS)
  {
    /* Handling the err cond when no mem avail to give data samples to SMGR */
    sns_dd_mag099xx_notify_smgr_err(state, SNS_DDF_ENOMEM, sensor_data_report, 1);
    goto COMPLETE_SAMPLE_SENSOR;
  }

  /* Now ready to send measurement data. Put it to the packet. */
  data_ptr[0].sample = state->data_cache[0];
  data_ptr[1].sample = state->data_cache[1];
  data_ptr[2].sample = state->data_cache[2];
  data_ptr[0].status = data_ptr[1].status = data_ptr[2].status = SNS_DDF_SUCCESS;

  /* mark data as ready */
  sensor_data_report->sensor      = SNS_DDF_SENSOR_MAG;
  sensor_data_report->status      = SNS_DDF_SUCCESS;
  sensor_data_report->timestamp   = sns_ddf_get_timestamp();
  sensor_data_report->samples     = data_ptr;
  sensor_data_report->num_samples = AKM_MAG099XX_NUM_DATATYPES;

  sns_dd_mag099xx_notify_smgr(state, sensor_data_report, 1);

  /** Logging and debug */
  sns_dd_mag099xx_log_data(state, sensor_data_report->timestamp);
  MAG_AKM_DBG_MSG_3(
      DBG_DD_MAG8975_GET_DATA_REQ,
      state->data_cache[0],
      state->data_cache[1],
      state->data_cache[2]);

  /* Free memory */
  sns_ddf_mfree(data_ptr);

COMPLETE_SAMPLE_SENSOR:
  /* Reset state regardless success */
  state->curr_state = SNSD_MAG_ST_IDLE;
  sns_dd_akm099xx_config_pwr_mode(
      state,
      SNS_DDF_POWERSTATE_LOWPOWER);

FINALIZE_SAMPLE_SENSOR:
  /* Free memory */
  sns_ddf_mfree(sensor_data_report);

  return;
}

/*==============================================================================

  FUNCTION:   sns_mgt_akm_fst_test_data

==============================================================================*/
static sns_ddf_status_e sns_mgt_akm_fst_test_data(
    uint16_t  testno,
    int16_t   testdata,
    int16_t   lolimit,
    int16_t   hilimit,
    uint32_t* err)
{
  if ((lolimit <= testdata) && (testdata <= hilimit)) {
    return SNS_DDF_SUCCESS;
  } else {
    *err = (uint32_t)((((uint32_t)testno)<<16)|((uint16_t)testdata));
    return SNS_DDF_EFAIL;
  }
}

#define AKM_FST(no, data, lo, hi, err) \
  if (sns_mgt_akm_fst_test_data((no),(data),(lo),(hi),(err)) \
      != SNS_DDF_SUCCESS) { goto TEST_SEQUENCE_FAILED; }

/*==============================================================================

  FUNCTION:   sns_mgt_akm_fst_sequence1

==============================================================================*/
/*!
  @brief Called when the timer set by this driver has expired.

  @note This function executes first step of Factory Shipment Test.
  This sequence should be started with ST_IDLE status.

  @param[in] *state   ptr to the driver state structure

  @return
   None

  $TODO:
*/
/*============================================================================*/
static void sns_mgt_akm_fst_sequence1(
    sns_dd_mag_akm099xx_state_s *state)
{
  uint8_t   write_buffer[2], out;
  uint8_t   read_buffer[AKM_MAG099XX_NUM_SENSITIVITY];
  uint32_t  err;

  /* Reset device */
  write_buffer[0] = AKM_MAG_SOFT_RESET;
  if (sns_ddf_write_port(
        state->port_handle,
        AKM_MAG_REG_ADDR_CNTL3_099XX,
        write_buffer,
        DDF_WRITE_ONE_BYTE,
        &out) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  /* After POR, wait for boot up the device (refer PORT in datasheet). */
  /* Wait for RESET operation completion. */
  sns_ddf_delay(100);

  /* Read Fuse ROM value */
  if (sns_mgt_akm_set_operation_mode_099xx(
        state,
        AKM_MAG_FUSE_ROM_ACCESS_MODE_099XX) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  /* Read Fuse ROM data */
  if (sns_ddf_read_port(
        state->port_handle,
        AKM_MAG_REG_ADDR_X_SEN_ADJUST_099XX,
        read_buffer,
        AKM_MAG099XX_NUM_SENSITIVITY,
        &out) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  /* Set power-down mode */
  if (sns_mgt_akm_set_operation_mode_099xx(
        state,
        AKM_MAG_PWR_DOWN_MODE_099XX) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  AKM_FST(TLIMIT_NO_ASAX, read_buffer[0], TLIMIT_LO_ASAX, TLIMIT_HI_ASAX, &err);
  AKM_FST(TLIMIT_NO_ASAY, read_buffer[1], TLIMIT_LO_ASAY, TLIMIT_HI_ASAY, &err);
  AKM_FST(TLIMIT_NO_ASAZ, read_buffer[2], TLIMIT_LO_ASAZ, TLIMIT_HI_ASAZ, &err);


  /* STEP 2 ================================================================*/
  /* Start self test */
  if (sns_mgt_akm_set_operation_mode_099xx(
        state,
        AKM_MAG_SELF_TEST_MODE_099XX) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  /* set state */
  state->curr_state = SNSD_MAG_ST_TEST_SEQ2;
  /* To be continued to sequence 2. */
  return;

TEST_SEQUENCE_FAILED:
  state->curr_state = SNSD_MAG_ST_IDLE;
  sns_dd_akm099xx_config_pwr_mode(
      state,
      SNS_DDF_POWERSTATE_LOWPOWER);
  sns_ddf_smgr_notify_test_complete(
      state->smgr_hndl,
      SNS_DDF_SENSOR_MAG,
      SNS_DDF_EFAIL,
      err);
  return;
}

/*==============================================================================

  FUNCTION:   sns_mgt_akm_fst_sequence2

==============================================================================*/
/*!
  @brief Called when the timer set by this driver has expired.

  @note This function executes second step of Factory Shipment Test.

  @param[in] *state   ptr to the driver state structure

  @return
   None

  $TODO:
*/
/*============================================================================*/
static void sns_mgt_akm_fst_sequence2(
    sns_dd_mag_akm099xx_state_s *state)
{
  uint8_t   out;
  uint8_t   akm_read_data[AKM_MAG099XX_NUM_DATA_BYTES];
  int16_t   xval_i16, yval_i16, zval_i16;
  uint32_t  err;

  /* Read ST1 first. Then check ST1 whether measurement is done or not */
  if (sns_ddf_read_port(
        state->port_handle,
        AKM_MAG_REG_ADDR_STATUS1_099XX,
        akm_read_data,
        DDF_READ_ONE_BYTE,
        &out) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  /* Check ST1 bit */
  if ((akm_read_data[0] & AKM_MAG_DRDY_BIT_MASK) == 0)
  {
    /* Measurement is not done yet. */
    if (NUM_MEASUREMENT_RETRY <= ++(state->retry))
    {
      /* When expire retry number, send error message. */
      err = SNSD_MAG_ERR_DRDY;
      goto TEST_SEQUENCE_FAILED;
    }
    /* Start timer again */
    if (sns_ddf_timer_start(
          state->sns_dd_mag_tmr_obj,
          USEC_TIME_FOR_MEASUREMENT) != SNS_DDF_SUCCESS)
    {
      err = SNSD_MAG_ERR_TIMER;
      goto TEST_SEQUENCE_FAILED;
    }
    /* No finalize sequence is needed. Just exit. */
    return;
  }

  /* Read ST1, 7 data registers, ST2 in a single 9-byte I2C transaction. */
  /* ST1 value shoud be 0, because this is second time to read out. */
  if (sns_ddf_read_port(
        state->port_handle,
        AKM_MAG_REG_ADDR_STATUS1_099XX,
        akm_read_data,
        AKM_MAG099XX_NUM_DATA_BYTES,
        &out) != SNS_DDF_SUCCESS)
  {
    err = SNSD_MAG_ERR_I2C;
    goto TEST_SEQUENCE_FAILED;
  }

  /* raw data in 16 bits */
  xval_i16 = (int16_t)(((uint16_t)akm_read_data[1]) | ((uint16_t)akm_read_data[2]<<8));
  yval_i16 = (int16_t)(((uint16_t)akm_read_data[3]) | ((uint16_t)akm_read_data[4]<<8));
  zval_i16 = (int16_t)(((uint16_t)akm_read_data[5]) | ((uint16_t)akm_read_data[6]<<8));
  /* Adjust sensitivity */
  xval_i16 = (int16_t)(xval_i16 * state->sens_adj[0]);
  yval_i16 = (int16_t)(yval_i16 * state->sens_adj[1]);
  zval_i16 = (int16_t)(zval_i16 * state->sens_adj[2]);
  /* Check read value */
  if (state->device_select == AKM_MAG09912_DEVICE_ID)
  {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09912, TLIMIT_HI_SLF_RVHX_09912, &err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09912, TLIMIT_HI_SLF_RVHY_09912, &err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09912, TLIMIT_HI_SLF_RVHZ_09912, &err);
  }
  else if (state->device_select == AKM_MAG09911_DEVICE_ID)
  {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09911, TLIMIT_HI_SLF_RVHX_09911, &err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09911, TLIMIT_HI_SLF_RVHY_09911, &err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09911, TLIMIT_HI_SLF_RVHZ_09911, &err);
  }
  else
  {
    err = SNSD_MAG_ERR_GENERIC;
    goto TEST_SEQUENCE_FAILED;
  }
  AKM_FST(TLIMIT_NO_SLF_ST2,  akm_read_data[8], TLIMIT_LO_SLF_ST2,  TLIMIT_HI_SLF_ST2,  &err);

  /* Reset the state to idle */
  state->curr_state = SNSD_MAG_ST_IDLE;
  sns_dd_akm099xx_config_pwr_mode(
      state,
      SNS_DDF_POWERSTATE_LOWPOWER);

  /* FST succeeded */
  sns_ddf_smgr_notify_test_complete(
      state->smgr_hndl,
      SNS_DDF_SENSOR_MAG,
      SNS_DDF_SUCCESS,
      SNSD_MAG_ERR_GENERIC);

  sns_dd_mag099xx_log_data(state, sns_ddf_get_timestamp());
  return;

TEST_SEQUENCE_FAILED:
  state->curr_state = SNSD_MAG_ST_IDLE;
  sns_dd_akm099xx_config_pwr_mode(
      state,
      SNS_DDF_POWERSTATE_LOWPOWER);
  sns_ddf_smgr_notify_test_complete(
      state->smgr_hndl,
      SNS_DDF_SENSOR_MAG,
      SNS_DDF_EFAIL,
      err);
  return;
}

/*###########################################################################
  *                    DDF APIs
 ###########################################################################*/

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_init

==============================================================================*/
/*!
  @brief Mag driver initialization.

  @detail
  Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
                             populate. This is a handle to the driver
                             instance that will be passed in to all
                             other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
                             calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
                             devices controlled by this driver. This is
                             used to configure the bus and talk to the
                             device.
  @param[in]  memhandler     Memory handler used to dynamically allocate
                             output parameters, if applicable. NB: Do
                             not use memhandler to allocate memory for
                             @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
                              returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
*/
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors)
{
  sns_dd_mag_akm099xx_state_s* state;
  sns_ddf_status_e             status;
  uint8_t                      device_id[2], out;
  
  *dd_handle_ptr = NULL;
  *sensors       = NULL;

  MAG_AKM_DBG_MSG_0(DBG_DD_MAG8975_INITIALIZING);

  if (sns_ddf_malloc(
        (void **)&state,
        sizeof(sns_dd_mag_akm099xx_state_s)) != SNS_DDF_SUCCESS)
  {
    status = SNS_DDF_ENOMEM;
    goto errorCleanUp_0;
  }

  memset(state, 0, sizeof(sns_dd_mag_akm099xx_state_s));

  if ((status = sns_ddf_open_port(
          &(state->port_handle),
          &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
    goto errorCleanUp_1;
  }

  /* Initialize state members in order of declaration. */
  state->smgr_hndl      = smgr_handle;
  state->dev_info       = device_info;
  state->curr_state     = SNSD_MAG_ST_IDLE;
  state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  
  drv_akm099xx_dbg = state; /* Debug Only */
  *dd_handle_ptr   = state;

  sns_ddf_axes_map_init(
      &state->axes_map,
      ((nv_params != NULL) ? nv_params->data : NULL));

  /* Init timer object */
  if ((status = sns_ddf_timer_init(
          &state->sns_dd_mag_tmr_obj,
          (sns_ddf_handle_t)state,
          &sns_mag_akm_driver_fn_list,
          state,
          false)) != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp_2;
  }

  /* Retry value is increased when operation is pending.
   * Therefore 0 is initial value. */
  state->retry = 0;

  /* Fill out supported sensor info */
  *sensors        = sns_dd_mag_sensor_types;
  *num_sensors    = AKM_MAG099XX_NUM_SUPPORTED_SENSORS;

  if (EnableI2C == FALSE)
    return SNS_DDF_SUCCESS;

  /* Read Device ID, to make sure device is working properly */
  if ((status = sns_ddf_read_port(
          (sns_ddf_handle_t)state->port_handle,
          AKM_MAG_REG_ADDR_WIA1_099XX,
          device_id,
          DDF_READ_TWO_BYTES,
          &out)) != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp_3;
  }

  if (device_id[0] != AKM_MAG_COMPANY_ID)
  {
    status = SNS_DDF_EDEVICE;
    goto errorCleanUp_3;
  }

  if ((device_id[1] != AKM_MAG09912_DEVICE_ID) &&
      (device_id[1] != AKM_MAG09911_DEVICE_ID))
  {
    status = SNS_DDF_EDEVICE;
    goto errorCleanUp_3;
  }
  state->device_select = device_id[1];
  
  /* Scale coefficient */
  if (state->device_select == AKM_MAG09912_DEVICE_ID)
  {
    state->mag_scale_flt = (AKM_MAG09912_SENSITIVITY * UTESLA_TO_GAUSS);
  }
  else if (state->device_select == AKM_MAG09911_DEVICE_ID)
  {
    state->mag_scale_flt = (AKM_MAG09911_SENSITIVITY * UTESLA_TO_GAUSS);
  }
  else
  {
    status = SNS_DDF_EDEVICE;
    goto errorCleanUp_3;
  }

  /* Read sensitivity data */
  if ((status = sns_mgt_akm_get_sensitivity_data_099xx(state))
      != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp_3;
  }

  return SNS_DDF_SUCCESS;

errorCleanUp_3:
  sns_ddf_timer_release(state->sns_dd_mag_tmr_obj);
errorCleanUp_2:
  sns_ddf_close_port(state->port_handle);
errorCleanUp_1:
  sns_ddf_mfree((void *)state);
errorCleanUp_0:
  /* Clear output arguments */
  *dd_handle_ptr = NULL;
  *sensors = NULL;
  *num_sensors = 0;
  /* Clear static variables */
  drv_akm099xx_dbg = NULL;
  return status;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_get_data

==============================================================================*/
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
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[])
{
  sns_ddf_status_e err_code;
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;

  /* state machine */
  switch (state->curr_state)
  {
  case SNSD_MAG_ST_IDLE:
    if ((err_code = sns_mgt_akm_set_operation_mode_099xx(
            state,
            AKM_MAG_SINGLE_MEAS_MODE_099XX)) == SNS_DDF_SUCCESS)
    {
      state->curr_state = SNSD_MAG_ST_STARTED;
      sns_dd_akm099xx_config_pwr_mode(
          state,
          SNS_DDF_POWERSTATE_ACTIVE);
      /* Mag returns data asynchronously. Hence set PENDING
       * status to return to Sensor Manager */
      err_code = SNS_DDF_PENDING;

#ifdef MAG_AKM_UNIT_TEST
#error code not present
#endif
    }
    break;

  case SNSD_MAG_ST_STARTED:
    /* This is added for completeness and making SMGR aware that it first
     * needs to change the HW device state to active and then get data which
     * in case of mag doesn't actually make a difference
     */
    err_code = SNS_DDF_PENDING;
    break;

  case SNSD_MAG_ST_TEST_SEQ1:
  case SNSD_MAG_ST_TEST_SEQ2:
    /* If self test is currently ongoing, streaming cannot be supported */
    err_code = SNS_DDF_EDEVICE_BUSY;
    break;

  default:
    err_code = SNS_DDF_EFAIL;
    break;
  }

  return err_code;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_set_attr

==============================================================================*/
/*!
  @brief Sets an attribute of the either the AKM09912 or AKM09911 mag

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Mag has only pwr attribute that can be changed.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] sensor     Sensor for which this attribute is to be set.
  @param[in] attrib     Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
   SNS_DDF_EINVALID_PARAM

  $TODO:
*/
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_set_attr(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value)
{
  sns_ddf_status_e             ret_val = SNS_DDF_SUCCESS;
  sns_dd_mag_akm099xx_state_s* state = (sns_dd_mag_akm099xx_state_s *)dd_handle;
  sns_ddf_powerstate_e         power_state;
  MAG_AKM_DBG_MSG_2(
      DBG_DD_MAG8975_SET_ATTRIB_REQ,
      sensor,
      attrib );

  if ((sensor != SNS_DDF_SENSOR_MAG) && (sensor != SNS_DDF_SENSOR__ALL))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      power_state = *(sns_ddf_powerstate_e *)value;
      ret_val = sns_dd_akm099xx_config_pwr_mode(state, power_state);
      break;

    case SNS_DDF_ATTRIB_RANGE:
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    case SNS_DDF_ATTRIB_LOWPASS:
    case SNS_DDF_ATTRIB_ODR:
    case SNS_DDF_ATTRIB_IO_REGISTER:
      /* AKM support only 1 set of range */
      /* AKM support only 1 resolution level */
      break;

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return(ret_val);
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_get_attr

==============================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - All the info of the devices is from their data sheet.

  @param[in] dd_handle    Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in] attrib       Attribute to be retrieved.
  @param[in] memhandler   Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_get_attr(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems)
{
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;

  MAG_AKM_DBG_MSG_2(
      DBG_DD_MAG8975_GET_ATTRIB_REQ,
      sensor,
      attrib);

  if (sensor != SNS_DDF_SENSOR_MAG)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch(attrib)
  {
  case SNS_DDF_ATTRIB_POWER_INFO:
    {
      sns_ddf_power_info_s* power_attrib;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_power_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      power_attrib  = (sns_ddf_power_info_s *)(*value);
      *num_elems    = 1;

      /* Value is unit of uA */
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        power_attrib->active_current    = AKM_MAG09912_HI_PWR;
        power_attrib->lowpower_current  = AKM_MAG09912_LO_PWR;
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        power_attrib->active_current    = AKM_MAG09911_HI_PWR;
        power_attrib->lowpower_current  = AKM_MAG09911_LO_PWR;
      }
    }
    break;

  case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_range_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_ranges  = (sns_ddf_range_s *)(*value);
      *num_elems     = 1;
      /* Value is unit of Gauss in Q16 format */
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        device_ranges->min  = AKM_MAG09912_MIN_RANGE;
        device_ranges->max  = AKM_MAG09912_MAX_RANGE;
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        device_ranges->min  = AKM_MAG09911_MIN_RANGE;
        device_ranges->max  = AKM_MAG09911_MAX_RANGE;
      }
    }
    break;
#ifndef USE_CC41
  case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s *device_res_adc;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_resolution_adc_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_res_adc  = (sns_ddf_resolution_adc_s *)(*value);
      *num_elems      = 1;
      /* Value is simple integer */
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        device_res_adc->bit_len  = AKM_MAG09912_RESOLUTION_ADC;
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        device_res_adc->bit_len  = AKM_MAG09911_RESOLUTION_ADC;
      }
      device_res_adc->max_freq = 50;
    }
    break;
#endif
  case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t *device_res;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_resolution_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_res  = (sns_ddf_resolution_t *)(*value);
      *num_elems  = 1;
#ifdef USE_CC41
      /* Value is simple integer */
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        *device_res = AKM_MAG09912_RESOLUTION_ADC;
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        *device_res = AKM_MAG09911_RESOLUTION_ADC;
      }
#else
      *device_res = FX_FLTTOFIX_Q16(state->mag_scale_flt);
#endif
    }
    break;
  case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay  = (sns_ddf_delays_s *)(*value);
      *num_elems    = 1;
      device_delay->time_to_active  = USEC_TIME_FOR_MEASUREMENT;
      device_delay->time_to_data    = USEC_TIME_FOR_MEASUREMENT;
    }
    break;
  case SNS_DDF_ATTRIB_DRIVER_INFO:
    {
      sns_ddf_driver_info_s *driver_info;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_driver_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      driver_info  = (sns_ddf_driver_info_s *)(*value);
      *num_elems   = 1;
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        driver_info->name = "AK09912";
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        driver_info->name = "AK09911";
      }
#ifdef USE_CC41
      driver_info->version = "1.0";
#else
      driver_info->version = 1;
#endif
    }
    break;
  case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      sns_ddf_device_info_s *device_info;
      if ((*value = sns_ddf_memhandler_malloc(
              memhandler,
              sizeof(sns_ddf_device_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_info  = (sns_ddf_device_info_s *)(*value);
      *num_elems   = 1;
      if (state->device_select == AKM_MAG09912_DEVICE_ID)
      {
        device_info->model = "AK09912";
      }
      else if (state->device_select == AKM_MAG09911_DEVICE_ID)
      {
        device_info->model = "AK09911";
      }
      device_info->vendor = "AKM";
      device_info->name = "Magnetometer";
#ifdef USE_CC41
      device_info->version = "1.0";
#else
      device_info->version = 1;
#endif
    }
    break;

  default:
    return SNS_DDF_EINVALID_PARAM;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_handle_timer

==============================================================================*/
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
/*============================================================================*/
static void sns_dd_akm099xx_handle_timer(
    sns_ddf_handle_t  dd_handle,
    void*             arg)
{
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;

  if (state->curr_state == SNSD_MAG_ST_TEST_SEQ1)
  {
    sns_mgt_akm_fst_sequence1(state);
  }
  else if (state->curr_state == SNSD_MAG_ST_TEST_SEQ2)
  {
    sns_mgt_akm_fst_sequence2(state);
  }
  else
  {
    sns_mgt_akm_sample_sensor_099xx(state);
  }
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_hdle_irq

==============================================================================*/
/*!
  @brief Called by the SMGR in response to an interrupt

  @detail

    @param[in] dd_handle  Handle to a driver instance.
    @param[in] irq        The IRQ representing the interrupt that occured.

  @return
   None

  $TODO:
*/
/*============================================================================*/
static void sns_dd_akm099xx_hdle_irq(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
  MAG_AKM_DBG_MSG_0(DBG_DD_MAG8975_HANDLE_IRQ_REQ);
  return;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_reset

==============================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  - resets some of the driver states

  @param[in] dd_handle   Handle to a driver instance.

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: If SPI interface is used, set I2CDIS register after soft reset.
*/
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_reset(
    sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e             status;
  sns_dd_mag_akm099xx_state_s* state = (sns_dd_mag_akm099xx_state_s *)dd_handle;
  uint8_t                      write_buffer_reset, out;

  /* Reset device */
  write_buffer_reset = AKM_MAG_SOFT_RESET;

  status = sns_ddf_write_port(state->port_handle,
      AKM_MAG_REG_ADDR_CNTL3_099XX,
      &write_buffer_reset,
      DDF_WRITE_ONE_BYTE,
      &out);

  /* After POR, wait for boot up the device (refer PORT). */
  sns_ddf_delay(100);

  /* Reset driver state (in order of struct declaration) */
  state->curr_state = SNSD_MAG_ST_IDLE;
  state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  sns_ddf_timer_cancel(state->sns_dd_mag_tmr_obj);
  state->retry = 0;

  return status;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_self_test

==============================================================================*/
/*!
  @brief Runs a factory self test case.
 
  Self tests cover an embedded hardware test in cases where the sensor
  supports it, as well as driver based sensor tests. This is generally run
  in a factory setting, and must not be called while a device is streaming
  data.
 
  @param[in] dd_handle  Handle to a driver instance.
  @param[in] sensor     Sensor on which to run the test.
  @param[in] test       Test case to run.
 
  @return One of the following error codes:
     SNS_DDF_SUCCESS        - Test passed.
     SNS_DDF_PENDING        - Test result will be sent as an event.
     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
     SNS_DDF_EFAIL          - Unknown error occured.
*/
/*============================================================================*/
sns_ddf_status_e sns_dd_akm099xx_self_test(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_test_e       test,
    uint32_t             *err)
{
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;

  /* set err code first */
  *err = 0;

  /* check parameter - sensor */
  if (sensor != SNS_DDF_SENSOR_MAG)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* check parameter - test */
  if (test != SNS_DDF_TEST_SELF)
  {
    return SNS_DDF_EINVALID_TEST;
  }

  /* check state */
  if (state->curr_state != SNSD_MAG_ST_IDLE)
  {
    MAG_AKM_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, state->curr_state);
    return SNS_DDF_EDEVICE_BUSY;
  }

  /* set state */
  state->curr_state = SNSD_MAG_ST_TEST_SEQ1;
  sns_dd_akm099xx_config_pwr_mode(
      state,
      SNS_DDF_POWERSTATE_ACTIVE);

  /* Start FST sequence */
  if (sns_ddf_timer_start(state->sns_dd_mag_tmr_obj, 0) != SNS_DDF_SUCCESS)
  {
    state->curr_state = SNSD_MAG_ST_IDLE;
    sns_dd_akm099xx_config_pwr_mode(
        state,
        SNS_DDF_POWERSTATE_LOWPOWER);
    return SNS_DDF_EFAIL;
  }

  /* To be continued to sequence 1. */ 
  return SNS_DDF_PENDING;
}

/*==============================================================================

  FUNCTION:   sns_dd_akm099xx_probe

==============================================================================*/
/*!
  @brief Probes for the device with a given configuration.
 
  This commands the driver to look for the device with the specified
  configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
  struct.
 
  @param[in]  dev_info    Access info for physicol devices controlled by
                          this driver. Used to determine if the device is
                          physically present.
  @param[in]  memhandler  Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] num_sensors Number of sensors supported. 0 if none.
  @param[out] sensor_type Array of sensor types supported, with num_sensor
                          elements. Allocated by this function.
 
  @return SNS_DDF_SUCCESS if the part was probed function completed, even
          if no device was found (in which case num_sensors will be set to
          0).
*/
/*============================================================================*/
static sns_ddf_status_e sns_dd_akm099xx_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t          read_buffer[2], bytes_read;

  /* "num = 0" means Nothing detected. */
  *num_sensors = 0;
  *sensors = NULL;
  /* No error code is reserved, but put something. */
  device_info->device_select = 0xFF;

  if ((status = sns_ddf_open_port(
          &port_handle,
          &(device_info->port_config))) != SNS_DDF_SUCCESS)
  {
    return status;
  }

  /* Read two bytes, i.e. WIA1 and WIA2 */
  if ((status = sns_ddf_read_port(
          port_handle,
          AKM_MAG_REG_ADDR_WIA1_099XX,
          read_buffer,
          DDF_READ_TWO_BYTES,
          &bytes_read)) != SNS_DDF_SUCCESS)
  {
    goto FINALIZE_PROBE;
  }

  /** 'status' should be SNS_DDF_SUCCESS at this line **/

  /* Verify Company ID */
  if (read_buffer[0] != AKM_MAG_COMPANY_ID)
  {
    /* Incorrect value. Return with nothing detected */
    goto FINALIZE_PROBE;
  }
  /* Check Device ID */
  if ((read_buffer[1] != AKM_MAG09912_DEVICE_ID) &&
      (read_buffer[1] != AKM_MAG09911_DEVICE_ID))
  {
    /* Unknown device. Return with nothing detected */
    goto FINALIZE_PROBE;
  }
  /* Put Device ID to 'device_info' struct for using in 'init' function */
  device_info->device_select = read_buffer[1];

  /* This is probably an AK099XX */
  *sensors = sns_ddf_memhandler_malloc(
      memhandler,
      sizeof(sns_ddf_sensor_e) * AKM_MAG099XX_NUM_SUPPORTED_SENSORS);

  if (*sensors == NULL)
  {
    status = SNS_DDF_ENOMEM;
    goto FINALIZE_PROBE;
  }

  /* set return value */
  (*sensors)[0] = SNS_DDF_SENSOR_MAG;
  *num_sensors = AKM_MAG099XX_NUM_SUPPORTED_SENSORS;

  status = SNS_DDF_SUCCESS;

FINALIZE_PROBE:
  sns_ddf_close_port(port_handle);
  return status;
}
#if 0
#ifdef AKM_DEBUG_TEST_MAIN
int main(int argc, char* argv[])
{
  sns_ddf_handle_t         dd_handle_ptr = NULL;
  sns_ddf_handle_t         smgr_handle = NULL;
  sns_ddf_nv_params_s      nv_params;
  sns_ddf_device_access_s  devinfo;
  sns_ddf_memhandler_s     memhandler;
  sns_ddf_sensor_e*        sensors;
  sns_ddf_sensor_data_s*   data[3];
  sns_ddf_status_e         status;
  uint32_t      numsensor;
  uint32_t      err;

#ifdef WIN32
  // Get current flag
  int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

  // Turn on leak-checking bit.
  tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

  // Set flag to the new value.
  _CrtSetDbgFlag( tmpFlag );
#endif

  sns_ddf_memhandler_init(&memhandler);

  status = sns_dd_akm099xx_probe(
      &devinfo,
      &memhandler,
      &numsensor,
      &sensors);
  if (status != SNS_DDF_SUCCESS)
  {
    printf("\nError: sns_dd_akm099xx_probe(%d).\n\n", status);
    goto SNS_DD_MAG_END;
  }

  status = sns_dd_akm099xx_init(
      &dd_handle_ptr,
      smgr_handle,
      &nv_params,
      &devinfo,
      1,
      &memhandler,
      &sensors,
      &numsensor);
  if (status != SNS_DDF_SUCCESS)
  {
    printf("\nError: sns_dd_akm099xx_init(%d).\n\n", status);
    goto SNS_DD_MAG_END;
  }

  status = sns_dd_akm099xx_self_test(
      dd_handle_ptr,
      sensors[0],
      SNS_DDF_TEST_SELF,
      &err);
  if ((status != SNS_DDF_SUCCESS) && (status != SNS_DDF_PENDING))
  {
    printf("\nError: sns_dd_akm099xx_self_test.\n\n");
    goto SNS_DD_MAG_END;
  }

  Sleep(1000);

#ifdef WIN32
  for(;;){
#else
  for(int i=0; i<20; i++){
#endif
    sns_dd_akm099xx_get_data(
        dd_handle_ptr,
        sensors,
        1,
        &memhandler,
        data);
    Sleep(100);
#ifdef WIN32
    if(_kbhit())
      break;
#endif
  }

  status = sns_dd_akm099xx_reset(dd_handle_ptr);
  if (status != SNS_DDF_SUCCESS)
  {
    printf("\nError: sns_dd_akm099xx_reset(%d).\n\n", status);
    goto SNS_DD_MAG_END;
  }

  printf("\nSampling done.\n\n");

SNS_DD_MAG_END:
  sns_ddf_memhandler_free(&memhandler);
  if(dd_handle_ptr != NULL){
    sns_ddf_timer_release(((sns_dd_mag_akm099xx_state_s *)(dd_handle_ptr))->sns_dd_mag_tmr_obj);
    sns_ddf_close_port(((sns_dd_mag_akm099xx_state_s *)(dd_handle_ptr))->port_handle);
    sns_ddf_mfree(dd_handle_ptr);
  }
  _getch();
  return 0;
}
#endif /* 0 */
#endif

