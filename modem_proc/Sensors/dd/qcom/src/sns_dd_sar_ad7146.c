/*============================================================================
 @file sns_dd_sar_ad7146.c

 @brief
 Source file for AD7146 capacitive touch sensor driver.

 DEPENDENCIES: None.

 Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential
 ============================================================================*/

/*============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order. Please
 use ISO format for dates.

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2014-11-05  ps   Replace memcpy from SNS_OS_MEMSCPY
 2014-08-08  MW   Eliminated KW erroe
 2014-05-16  MW   Updated sns_dd_ad7146_self_test()
 2014-05-08  RT   Fixed reset issue
 2014-04-25  MW   Eliminate compiler warning
 2014-03-12  MW   Fixed KW issue
 2014-02-14  MW   Added self test, timer for polling mode
 2014-02-10  TC   Added support for dynamic stage configuration through the registry
 2014-02-04  MW   Initial indication fix CR 611348
 2013-11-18  MW   Fix sns_ddf_close_port calls
 2013-10-30  MW   Close ddf port before freeing state variable. Threshold updates.
 2013-09-16  MW   Initial version
 ============================================================================*/

#include <stdio.h>
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_memmgr.h"

#include "sns_dd_sar_ad7146.h"

#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

// Forward declaration of driver interface functions.
sns_ddf_driver_if_s sns_dd_sar_ad7146_fn_list;

/*===========================================================================*/
/*
 *  Local functions
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_change_power_mode(
    sns_ddf_handle_t handle,
    int power_mode);

static sns_ddf_status_e sns_dd_ad7146_enable_interrupt(sns_ddf_handle_t handle);

static sns_ddf_status_e sns_dd_ad7146_disable_interrupt(sns_ddf_handle_t handle);

static sns_ddf_status_e sns_dd_ad7146_config_afe(sns_ddf_handle_t dd_handle, 
                                           uint32_t stage, bool *afe_changed);

/*===========================================================================
 ===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_burst_read_register(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t          *data,
    uint8_t          read_count)
{
  sns_dd_ad7146_state_t* state = handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint8_t reg_val[2];
  uint8_t bytes_write = 0;

  reg_val[1] = reg_addr & 0xFF;
  reg_val[0] = (reg_addr >> 8) & 0xFF;

  ret = sns_ddf_simple_write_port(state->port_handle, (uint8_t*) &reg_val[0], 2,
      &bytes_write);

  if ((ret != SNS_DDF_SUCCESS) || (bytes_write != 2))
  {
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);
    return ret;
  }

  ret = sns_ddf_simple_read_port(state->port_handle, (uint8_t*) &data[0], read_count,
      &bytes_write);

  if ((ret != SNS_DDF_SUCCESS) || (bytes_write != read_count))
  {
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);
    return ret;
  }

  return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e sns_dd_ad7146_read_register(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint16_t          *data)
{
  sns_dd_ad7146_state_t* state = handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint8_t reg_val[2];
  uint8_t bytes_write = 0;

  reg_val[1] = reg_addr & 0xFF;
  reg_val[0] = (reg_addr >> 8) & 0xFF;

  ret = sns_ddf_simple_write_port(state->port_handle, (uint8_t*) &reg_val[0], 2,
      &bytes_write);

  if ((ret != SNS_DDF_SUCCESS) || (bytes_write != 2))
  {
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);
    return ret;
  }

  ret = sns_ddf_simple_read_port(state->port_handle, (uint8_t*) &reg_val[0], 2,
      &bytes_write);

  if ((ret != SNS_DDF_SUCCESS) || (bytes_write != 2))
  {
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);
    return ret;
  }

  *data = (uint16_t) ((reg_val[0] << 8) | reg_val[1]);

  return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_ad7146_write_register(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint16_t          data)
{
  sns_dd_ad7146_state_t* state = handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint8_t reg_val[4];
  uint8_t bytes_write = 0;

  reg_val[0] = (reg_addr >> 8) & 0xFF;
  reg_val[1] = reg_addr & 0xFF;
  reg_val[2] = (data >> 8) & 0xFF;
  reg_val[3] = data & 0xFF;

  ret = sns_ddf_simple_write_port(state->port_handle, (uint8_t*) &reg_val[0], 4,
      &bytes_write);

  if ((ret != SNS_DDF_SUCCESS) || (bytes_write != 4))
  {
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);
    return ret;
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================
 FUNCTION      sns_dd_ad7146_log_data

 DESCRIPTION   Log the latest sensor data

 DEPENDENCIES  None

 RETURN VALUE  None

 SIDE EFFECT   None
 ===========================================================================*/
static void sns_dd_ad7146_log_data(
    sns_dd_ad7146_state_t*  dd_ptr,
    uint32_t                cap_touch_sensor,
    uint32_t                cap_touch_data)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s * log_struct_ptr;

  // Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s), (void**) &log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL ))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_SAR;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_ADI;

    /* Log the communication info to correctly identify which SAR this log
     * packet belongs to.*/
    if (dd_ptr->port_info.bus == SNS_DDF_BUS_I2C)
    {
      log_struct_ptr->bus_type = SNS_LOG_SENSOR_BUS_TYPE_I2C;
      log_struct_ptr->bus_inst = dd_ptr->port_info.bus_inst;
      log_struct_ptr->dev_addr = dd_ptr->port_info.dev_addr;
    }
    else
    {
      log_struct_ptr->bus_type = SNS_LOG_SENSOR_BUS_TYPE_NONE;
      log_struct_ptr->bus_inst = 0;
      log_struct_ptr->dev_addr = 0;
    }

    // Timestamp the log with sample time
    log_struct_ptr->timestamp = sns_ddf_get_timestamp();

    // Log sensor data
    // Two data items for both humidity and temperature sensor types
    log_struct_ptr->num_data_types = 2;
    log_struct_ptr->data[0] = cap_touch_sensor;
    log_struct_ptr->data[1] = cap_touch_data;

    // Commit log (also frees up the log packet memory)
    err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log_struct_ptr);
    if (err_code != SNS_SUCCESS)
    {
      AD7146_MSG_0(ERROR, "sns_logpkt_commit - error");
    }
  }
}

#ifdef AD7146_DEBUG

static void sns_dd_ad7146_read_stage_0_reg(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t reg_data;

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_DEVICE_ID_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_DEVICE_ID_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE0_CONNECTION_6_0_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_CONNECTION_6_0_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE0_CONNECTION_12_7_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_CONNECTION_12_7_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE0_AFE_OFFSET_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_AFE_OFFSET_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE0_SENSITIVITY_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_SENSITIVITY_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE0_OFFSET_LOW_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_OFFSET_LOW_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE0_OFFSET_HIGH_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_OFFSET_HIGH_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE0_OFFSET_HIGH_CLAMP_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_OFFSET_HIGH_CLAMP_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE0_OFFSET_LOW_CLAMP_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE0_OFFSET_LOW_CLAMP_REG = %X", reg_data);

  if(ret != SNS_DDF_SUCCESS)
  {
    // do nothing. this is to eliminate compiler warning
  }
}

static void sns_dd_ad7146_read_stage_1_reg(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t reg_data;

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE1_CONNECTION_6_0_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_CONNECTION_6_0_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE1_CONNECTION_12_7_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_CONNECTION_12_7_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE1_AFE_OFFSET_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_AFE_OFFSET_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE1_SENSITIVITY_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_SENSITIVITY_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE1_OFFSET_LOW_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_OFFSET_LOW_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE1_OFFSET_HIGH_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_OFFSET_HIGH_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE1_OFFSET_HIGH_CLAMP_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_OFFSET_HIGH_CLAMP_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE1_OFFSET_LOW_CLAMP_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE1_OFFSET_LOW_CLAMP_REG = %X", reg_data);

  if(ret != SNS_DDF_SUCCESS)
  {
    // do nothing. this is to eliminate compiler warning
  }
}

static void sns_dd_ad7146_read_stage_2_reg(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t reg_data;

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE2_CONNECTION_6_0_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_CONNECTION_6_0_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE2_CONNECTION_12_7_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_CONNECTION_12_7_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE2_AFE_OFFSET_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_AFE_OFFSET_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE2_SENSITIVITY_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_SENSITIVITY_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE2_OFFSET_LOW_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_OFFSET_LOW_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE2_OFFSET_HIGH_REG,
      &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_OFFSET_HIGH_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE2_OFFSET_HIGH_CLAMP_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_OFFSET_HIGH_CLAMP_REG = %X", reg_data);

  ret = sns_dd_ad7146_read_register(dd_handle,
      AD7146_STAGE2_OFFSET_LOW_CLAMP_REG, &reg_data);
  AD7146_MSG_1(MED, "AD7146_STAGE2_OFFSET_LOW_CLAMP_REG = %X", reg_data);

  if(ret != SNS_DDF_SUCCESS)
  {
    // do nothing. this is to eliminate compiler warning
  }
}

#endif // AD7146_DEBUG

static void sns_dd_ad7146_read_bank_1_reg(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint8_t reg_data_array[SAR_AD7146_NUM_BANK1_REG];
  int i;

#ifdef AD7146_DEBUG
  // Read all bank 1 registers when debugging
  ret = sns_dd_ad7146_burst_read_register(
    dd_handle,
    AD7146_PWR_CONTROL_REG,
    &reg_data_array[0],
    SAR_AD7146_NUM_BANK1_REG);

  for(i=0 ; i<SAR_AD7146_NUM_BANK1_REG ; )
  {
    AD7146_MSG_2(MED, "reg %d = %X", (i/SAR_AD7146_BYTES_PER_REG), (uint16_t) ((reg_data_array[i] << 8) | reg_data_array[i+1]));
    i += SAR_AD7146_BYTES_PER_REG;
  }
#endif

  // read AD7146_STAGE_LOW_INT_STATUS_REG, AD7146_STAGE_HIGH_INT_STATUS_REG and 
  // AD7146_STAGE_COMPLETE_INT_STATUS_REG to clear pending interrupts
  ret = sns_dd_ad7146_burst_read_register(
    dd_handle,
    AD7146_STAGE_LOW_INT_STATUS_REG,
    &reg_data_array[0],
    (SAR_AD7146_NUM_STATUS_REG*SAR_AD7146_BYTES_PER_REG));

  for(i=0 ; i<(SAR_AD7146_NUM_STATUS_REG*SAR_AD7146_BYTES_PER_REG) ; )
  {
    AD7146_MSG_2(MED, "reg %d = %X", (i/SAR_AD7146_BYTES_PER_REG), (uint16_t) ((reg_data_array[i] << 8) | reg_data_array[i+1]));
    i += SAR_AD7146_BYTES_PER_REG;
  }
  
  if(ret != SNS_DDF_SUCCESS)
  {
    // Do nothing. This is to eliminate compiler warning.
  }
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_configure_stages: Configures all stages
 *
 * A stage is where capacitance from one (or more electrodes, in parallel) is
 * measured and compared against some threshold value. For all intents and
 * purposes, it can be treated as an electrode.
 *
 * @param[in] dd_handle Handle to driver instance.
 *
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_configure_stages(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *) dd_handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t offset_low_clamp_value;
  uint16_t offset_high_clamp_value;

  uint16_t connection_6_0_reg = AD7146_STAGEx_CONNECTION_6_0_REG_BASE;
  uint16_t connection_12_7_reg = AD7146_STAGEx_CONNECTION_12_7_REG_BASE;
  uint16_t afe_offset_reg = AD7146_STAGEx_AFE_OFFSET_REG_BASE;
  uint16_t sensitivity_reg = AD7146_STAGEx_SENSITIVITY_REG_BASE;
  uint16_t offset_low_reg = AD7146_STAGEx_OFFSET_LOW_REG_BASE;
  uint16_t offset_high_reg = AD7146_STAGEx_OFFSET_HIGH_REG_BASE;
  uint16_t offset_low_clamp_reg = AD7146_STAGEx_OFFSET_LOW_CLAMP_REG_BASE;
  uint16_t offset_high_clamp_reg = AD7146_STAGEx_OFFSET_HIGH_CLAMP_REG_BASE;

  uint8_t i;

  sns_dd_ad7146_config_s * config = &state->stage_config;

  // Set up each stage
  for (i = 0; i < state->stage_config.number_of_stages; i++)
  {
    // Clamp values should be 120% of the Offset values
    offset_low_clamp_value = (uint16_t) (1.20 * config->stage_config_aux_array[i].offset_low);
    offset_high_clamp_value = (uint16_t) (1.20 * config->stage_config_main_array[i].offset_high);

    /* Write the values to the registers */
    ret = sns_dd_ad7146_write_register(dd_handle, connection_6_0_reg,
        config->stage_config_aux_array[i].connection_6_0);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    ret = sns_dd_ad7146_write_register(dd_handle, connection_12_7_reg,
        config->stage_config_aux_array[i].connection_12_7);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    ret = sns_dd_ad7146_write_register(dd_handle, afe_offset_reg,
        config->stage_config_main_array[i].afe_offset);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    // Sensitivity at midrange
    ret = sns_dd_ad7146_write_register(dd_handle, sensitivity_reg,
        AD7146_STAGEx_SENSITIVITY_VALUE);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    // Initial offset low and high values
    ret = sns_dd_ad7146_write_register(dd_handle, offset_low_reg,
        config->stage_config_aux_array[i].offset_low);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    ret = sns_dd_ad7146_write_register(dd_handle, offset_high_reg,
        config->stage_config_main_array[i].offset_high);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    // Initial offset low and high clamp values
    ret = sns_dd_ad7146_write_register(dd_handle, offset_high_clamp_reg,
        offset_high_clamp_value);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    ret = sns_dd_ad7146_write_register(dd_handle, offset_low_clamp_reg,
        offset_low_clamp_value);
    if (ret != SNS_DDF_SUCCESS)
    {
      AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_stages ret=%d", ret);
      return ret;
    }

    // Increment the register addresses to the next stage's register address so
    // that the next iteration of this loop will have the right register addresses
    connection_6_0_reg += AD7146_STAGE_INCREMENT;
    connection_12_7_reg += AD7146_STAGE_INCREMENT;
    afe_offset_reg += AD7146_STAGE_INCREMENT;
    sensitivity_reg += AD7146_STAGE_INCREMENT;
    offset_low_reg += AD7146_STAGE_INCREMENT;
    offset_high_reg += AD7146_STAGE_INCREMENT;
    offset_low_clamp_reg += AD7146_STAGE_INCREMENT;
    offset_high_clamp_reg += AD7146_STAGE_INCREMENT;
  }

  return ret;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_configure_bank_1: Configures memory bank 1
 *
 * @param[in] dd_handle Handle to driver instance.
 *
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_configure_bank_1(
    sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *) dd_handle;
  uint16_t pwr_con;
  uint16_t num_stages;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_configure_bank_1 - start");

  // CDC bias + 50%, enable excitation to CINx, interrupt active low,
  // 256 decimation rate, 200ms low power mode conv delay, full shutdown
  pwr_con = AD7146_PWR_CONTROL_VALUE_DEFAULT;

  /**
   * Configure the number of stages to be configured for the power control.
   *
   * From the datasheet:
   * [7:4] - Number of stages in sequence (N + 1)
   *         0000 = 1 conversion stage in sequence
   *         0001 = 2 conversion stage in sequence
   *         ...
   */
  num_stages = (uint16_t) (state->stage_config.number_of_stages - 1);
  num_stages = num_stages << 4;
  pwr_con |= num_stages;

  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_PWR_CONTROL_REG,
      pwr_con);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  // Environmental compensation registers, not used in this application, use default values
  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_AMB_COMP_CTRL0_REG,
      state->stage_config.comp_control_0);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_AMB_COMP_CTRL1_REG,
      state->stage_config.comp_control_1);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_AMB_COMP_CTRL2_REG,
      state->stage_config.comp_control_2);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  // Disable low interrupt for all stages
  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_STAGE_LOW_INT_ENABLE_REG,
      0x0000);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  // Disable high interrupt for all stages
  ret = sns_dd_ad7146_write_register(dd_handle,
      AD7146_STAGE_HIGH_INT_ENABLE_REG, 0x0000);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  // Disable conversion complete interrupt for all stages
  ret = sns_dd_ad7146_write_register(dd_handle,
      AD7146_STAGE_COMPLETE_INT_ENABLE_REG, 0x0000);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_configure_bank_1 ret=%d", ret);
    return ret;
  }

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_configure_bank_1 - end");

  return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_parse_nv_params: Translates and stores the NV params
 *
 *
 *
 * @param[in] state      Pointer to the driver instance.
 * @param[in] nv_params  Pointer to the NV Params, passed into the init function
 */
/*===========================================================================*/
static void sns_dd_ad7146_parse_nv_params(
    sns_dd_ad7146_state_t*  state,
    sns_ddf_nv_params_s*    nv_params)
{
  if (state == NULL || nv_params == NULL )
  {
    AD7146_MSG_2(ERROR, "Unable to parse NV params. state 0x%x nv_params 0x%x",
        state, nv_params);
    return;
  }

  sns_dd_ad7146_config_s* config = &state->stage_config;

  // Copy the configuration
  SNS_OS_MEMSCPY(config, sizeof(sns_dd_ad7146_config_s), nv_params->data,
      FX_MIN(nv_params->data_len, sizeof(sns_dd_ad7146_config_s)));
  AD7146_MSG_1(LOW, "Copied %d bytes to the configuration",
      FX_MIN(nv_params->data_len, sizeof(sns_dd_ad7146_config_s)));

  // Validate the input
  if (config->number_of_stages > SAR_AD7146_MAX_NUM_OF_STAGES)
  {
    AD7146_MSG_2(HIGH, "Number of electrodes (%d) too high, setting to max %d",
        config->number_of_stages, SAR_AD7146_MAX_NUM_OF_STAGES);
    config->number_of_stages = SAR_AD7146_MAX_NUM_OF_STAGES;
  }
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_reset: This function:
 *				1. Software resets the AD7146
 *				2. Configures input
 *				3. Puts AD7146 in full shutdown mode
 *				4. Resets driver state
 *
 * @param[in] dd_handle Handle to driver instance.
 *
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_ad7146_reset(sns_ddf_handle_t dd_handle)
{
  sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *) dd_handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t cal_en;
  uint16_t reg_data;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_reset - enter");

  /* basic sanity check */
  if (dd_handle == NULL )
    return SNS_DDF_EINVALID_PARAM;

  // Write the value 0x0FFF to the calibration register before the reset
  // This is later used to verify that the software reset was successful
  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_STAGE_CAL_EN_REG, 0x0FFF);
  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_reset ret=%d", ret);
    return ret;
  }

  // Software reset - ignore I2C Bus errors on a Reset as the part may respond with a NACK
  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_PWR_CONTROL_REG, 0x0400);
  if ( (ret != SNS_DDF_SUCCESS) && (ret != SNS_DDF_EBUS) )
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_reset ret=%d", ret);
    return ret;
  }

  // 10 ms delay
  sns_ddf_delay(10000);

  // Make sure the reset was successful by reading the calibration register
  ret = sns_dd_ad7146_read_register(dd_handle, AD7146_STAGE_CAL_EN_REG, &reg_data);
  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_reset ret=%d", ret);
    return ret;
  }
  // If the calibration register is not 0, the reset was not successful
  if (reg_data != 0x0)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_reset reg_data=%d", reg_data);
    return SNS_DDF_EFAIL;
  }

  ret = sns_dd_ad7146_configure_stages(dd_handle);
  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "Unable to configure stages. reason: %d", ret);
    return ret;
  }

  ret = sns_dd_ad7146_configure_bank_1(dd_handle);
  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_reset ret=%d", ret);
    return ret;
  }

  // calibration enable
  cal_en = (0x0001 << state->stage_config.number_of_stages) - 1;
  ret = sns_dd_ad7146_write_register(dd_handle, AD7146_STAGE_CAL_EN_REG,
      cal_en);
  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_reset ret=%d", ret);
    return ret;
  }

  state->is_data_requested = false;
  state->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  state->first_indication_count = 0;
  state->first_indication_sent = false;
  state->run_calibration = false;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_reset - exit");

  return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_init Initializes the AD7146 device driver
 *
 * @detail Allocates handle for this driver instance.
 *         Opens I2C communication port.
 *         Places device in default power state.
 *         This function will be called at init time.
 *
 *  @param[out] dd_handle_ptr  Pointer that this function must malloc and
 *                             populate. This is a handle to the driver
 *                             instance that will be passed in to all other
 *                             functions. NB: Do not use @a memhandler to
 *                             allocate this memory.
 *  @param[in]  smgr_handle    Handle used to identify this driver when it
 *                             calls into Sensors Manager functions.
 *  @param[in]  nv_params      NV parameters retrieved for the driver.
 *  @param[in]  device_info    Access info for physical devices controlled by
 *                             this driver. Used to configure the bus
 *                             and talk to the devices.
 *  @param[in]  num_devices    Number of elements in @a device_info.
 *  @param[in]  memhandler     Memory handler used to dynamically allocate
 *                             output parameters, if applicable. NB: Do not
 *                             use memhandler to allocate memory for
 *                             @a dd_handle_ptr.
 *  @param[out] sensors        List of supported sensors, allocated,
 *                             populated, and returned by this function.
 *  @param[out] num_sensors    Number of elements in @a sensors.
 *
 *  @return Success if @a dd_handle_ptr was allocated and the driver was
 *          configured properly. Otherwise a specific error code is returned.
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_ad7146_init(
    sns_ddf_handle_t*         dd_ptr,
    sns_ddf_handle_t          smgr_handle,
    sns_ddf_nv_params_s*      reg_params,
    sns_ddf_device_access_s   device_info[],
    uint32_t                  num_devices,
    sns_ddf_memhandler_s*     memhandler,
    sns_ddf_sensor_e*         sensors[],
    uint32_t*                 num_sensors)
{
  sns_dd_ad7146_state_t* state;
  sns_ddf_status_e ret;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_init - enter");

  /* basic sanity check */
  if (dd_ptr == NULL || sensors == NULL || num_sensors == NULL || device_info == NULL)
    return SNS_DDF_EINVALID_PARAM;

  /* allocate state memory */
  if (sns_ddf_malloc((void **) &state, sizeof(sns_dd_ad7146_state_t)) != SNS_DDF_SUCCESS)
  {
    return SNS_DDF_ENOMEM;
  }

  if (sns_ddf_malloc((void **) &state->s_data, sizeof(sns_ddf_sensor_data_s)) != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(state);
    return SNS_DDF_ENOMEM;
  }

  // Allocate enough memory for each of the axes
  if (sns_ddf_malloc((void **) &state->s_data->samples, 
         SAR_AD7146_MAX_NUM_OF_STAGES * SAR_AD7146_NUM_OF_AXES * sizeof(sns_ddf_sensor_sample_s))
      != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(state->s_data);
    sns_ddf_mfree(state);
    return SNS_DDF_ENOMEM;
  }

  state->smgr_handle = smgr_handle;

  // change I2C config to use 16-bit register addresses
  //device_info[0].port_config.bus_config.i2c->reg_addr_type = SNS_DDF_I2C_REG_ADDR_16BIT;

  /* open I2C port */
  ret = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
  if (ret != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(state->s_data->samples);
    sns_ddf_mfree(state->s_data);
    sns_ddf_mfree(state);
    return ret;
  }

  state->port_info.bus = device_info[0].port_config.bus;
  state->port_info.bus_inst = device_info[0].port_config.bus_instance;

  if (device_info[0].port_config.bus == SNS_DDF_BUS_I2C)
  {
    state->port_info.dev_addr = device_info[0].port_config.bus_config.i2c->slave_addr;
  }

  sns_dd_ad7146_parse_nv_params(state, reg_params);

  ret = sns_dd_ad7146_reset(state);

  if (ret != SNS_DDF_SUCCESS)
  {
    sns_ddf_close_port(state->port_handle);
    sns_ddf_mfree(state->s_data->samples);
    sns_ddf_mfree(state->s_data);
    sns_ddf_mfree(state);
    return ret;
  }

  *dd_ptr = (sns_ddf_handle_t) state;
  *num_sensors = 2;
  *sensors = (sns_ddf_sensor_e *) sns_dd_ad7146_sensor_types;

  ret = sns_ddf_timer_init(&state->sar_timer,
                            state,
                            &sns_dd_sar_ad7146_fn_list,
                            state,
                            false);

  if (ret != SNS_DDF_SUCCESS)
  {
    sns_ddf_close_port(state->port_handle);
    sns_ddf_mfree(state->s_data->samples);
    sns_ddf_mfree(state->s_data);
    sns_ddf_mfree(state);
    return ret;
  }

  state->interrupt_gpio = device_info->first_gpio;

  if (state->interrupt_gpio != 0xFFFF)
  {
    state->dri_enable = true;
  }
  else
  {
    state->dri_enable = false;
  }

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_init - exit");

  return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_get_data: Requests a single sample of sensor data.
 *       Data is returned immediately after being read from the sensor.
 *
 * @param[in]  dd_handle    Handle to driver instance.
 * @param[in]  sensors      List of sensors for which data is requested.
 * @param[in]  num_sensors  Length of @a sensors.
 * @param[in]  memhandler   Memory handler used to dynamically allocate
 *                          output parameters, if applicable.
 * @param[out] data         Sampled sensor data.

 * @return SNS_DDF_SUCCESS if data was populated successfully, error code
 *         otherwise.
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_ad7146_get_data(
    sns_ddf_handle_t          dd_handle,
    sns_ddf_sensor_e          sensors[],
    uint32_t                  num_sensors,
    sns_ddf_memhandler_s*     memhandler,
    sns_ddf_sensor_data_s**   data)
{
  sns_dd_ad7146_state_t *state = (sns_dd_ad7146_state_t *) dd_handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;

  /* basic sanity check */
  if (dd_handle == NULL || sensors == NULL || num_sensors != 1||
  memhandler == NULL || data == NULL)
    return SNS_DDF_EINVALID_PARAM;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_get_data - enter");

  state->is_data_requested = true;
  state-> timestamp = sns_ddf_get_timestamp();

  ret = sns_ddf_timer_start(state->sar_timer, 
        (state->stage_config.number_of_stages * AD7146_STAGE_CONVERSION_DELAY_US));
  if(ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_0(ERROR,"sns_dd_ad7146_get_data - timer start failed");
    return ret;
  }

  return SNS_DDF_PENDING;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_set_attrib: Sets certain device attributes that
 are programmable. Currently only power mode is a settable attribute.
 *
 * @param[in] dd_handle  Handle to driver instance.
 * @param[in] sensor     Sensor for which this attribute is to be set.
 * @param[in] attrib     Attribute to be set.
 * @param[in] value      Value to set this attribute.
 *
 *  @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_ad7146_set_attrib(
    sns_ddf_handle_t      dd_handle,
    sns_ddf_sensor_e      sensor,
    sns_ddf_attribute_e   attrib,
    void*                 value)
{
  sns_dd_ad7146_state_t * state = (sns_dd_ad7146_state_t *) dd_handle;
  /* basic sanity check */
  if (dd_handle == NULL || value == NULL )
    return SNS_DDF_EINVALID_PARAM;

  AD7146_MSG_3(HIGH,
      "sns_dd_ad7146_set_attrib - enter sensor=%d, attrib=%d, value=%d", sensor,
      attrib, *(uint8_t *)value);

  /* set attribute */
  switch (attrib)
  {
  case SNS_DDF_ATTRIB_POWER_STATE:
    return sns_dd_ad7146_change_power_mode(dd_handle,
        *(sns_ddf_powerstate_e *) value);

  case SNS_DDF_ATTRIB_RANGE:
    /* Does not support different ranges */
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_ODR:
    state->odr = SAR_AD7146_ODR;
    return SNS_DDF_SUCCESS;

  default:
    return SNS_DDF_EINVALID_PARAM;
  }

  return SNS_DDF_EINVALID_PARAM;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_get_attrib: Get device/driver attributes.
 *
 * @param[in] dd_handle  Handle to driver instance.
 * @param[in] sensor     Sensor for which this attribute is requested.
 * @param[in] attrib     Attribute to be get.
 * @param[in] memhandler Memory allocation resource.
 * @param[out] value     Value to set this attribute.
 * @param[out] num_elems  Number of elements in output value.
 *
 *  @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_ad7146_get_attrib(
    sns_ddf_handle_t        handle,
    sns_ddf_sensor_e        sensor,
    sns_ddf_attribute_e     attrib,
    sns_ddf_memhandler_s*   memhandler,
    void**                  value,
    uint32_t*               num_elems)
{
  sns_dd_ad7146_state_t *state = (sns_dd_ad7146_state_t *) handle;
  sns_ddf_driver_info_s *driver_info;
  sns_ddf_device_info_s *device_info;
  sns_ddf_range_s *device_ranges;
  sns_ddf_resolution_t *resolution;
  sns_ddf_resolution_adc_s *resolution_adc;
  sns_ddf_power_info_s *power;
  sns_ddf_powerstate_e *power_state;
  uint32_t *odr;
  sns_ddf_registry_group_s  *reg_group_ptr;
  uint8_t                   *reg_group_data_ptr;

  AD7146_MSG_2(HIGH, "sns_dd_ad7146_get_attrib - enter sensor=%d, attrib=%d",
      sensor, attrib);

  /* basic sanity check */
  if (handle == NULL || memhandler == NULL || value == NULL
      || num_elems == NULL )
    return SNS_DDF_EINVALID_PARAM;

  /* return attribute */
  switch (attrib)
  {
  case SNS_DDF_ATTRIB_DRIVER_INFO:
    driver_info = sns_ddf_memhandler_malloc(memhandler, sizeof(*driver_info));
    if (driver_info == NULL )
      return SNS_DDF_ENOMEM;
    driver_info->name = "AD1746";
    driver_info->version = 2;
    *value = driver_info;
    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_DEVICE_INFO:
    device_info = sns_ddf_memhandler_malloc(memhandler, sizeof(*device_info));
    if (device_info == NULL )
      return SNS_DDF_ENOMEM;
    device_info->name = "SAR Sensor";
    device_info->vendor = "ADI";
    device_info->model = "AD1746";
    device_info->version = 1;
    *value = device_info;
    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_RANGE:
    device_ranges = sns_ddf_memhandler_malloc(memhandler,
        sizeof(*device_ranges));
    if (device_ranges == NULL )
      return SNS_DDF_ENOMEM;

    device_ranges->min = FX_FLTTOFIX_Q16(0);
    device_ranges->max = FX_FLTTOFIX_Q16(5.0);
    *value = device_ranges;
    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_RESOLUTION:
    resolution = sns_ddf_memhandler_malloc(memhandler, sizeof(*resolution));
    if (resolution == NULL )
      return SNS_DDF_ENOMEM;

    *resolution = FX_FLTTOFIX_Q16(1);
    *value = resolution;

    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    resolution_adc = sns_ddf_memhandler_malloc(memhandler,
        sizeof(*resolution_adc));
    if (resolution_adc == NULL )
      return SNS_DDF_ENOMEM;

    *value = resolution_adc;
    *num_elems = 1;
    resolution_adc->bit_len = SAR_AD7146_ADC_BIT_LENGTH;
    resolution_adc->max_freq = SAR_AD7146_MAX_FREQ;

    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_POWER_INFO:
    power = sns_ddf_memhandler_malloc(memhandler, sizeof(*power));
    if (power == NULL )
      return SNS_DDF_ENOMEM;

    power->active_current = SAR_AD7146_ACTIVE_CURRENT;
    power->lowpower_current = SAR_AD7146_LOW_CURRENT;
    *value = power;
    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_POWER_STATE:
    power_state = sns_ddf_memhandler_malloc(memhandler, sizeof(*power_state));
    if (power_state == NULL )
      return SNS_DDF_ENOMEM;

    *power_state = state->powerstate;
    *value = power_state;

    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_ODR:
    odr = sns_ddf_memhandler_malloc(memhandler, sizeof(*odr));
    if (odr == NULL )
    {
      AD7146_MSG_0(HIGH, "ODR memory error");
      return SNS_DDF_ENOMEM;
    }

    *odr = state->odr;
    *value = odr;
    *num_elems = 1;
    return SNS_DDF_SUCCESS;

  case SNS_DDF_ATTRIB_REGISTRY_GROUP:
   if((reg_group_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL)
   {
    return SNS_DDF_ENOMEM;
   }

   if ( (reg_group_data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_dd_ad7146_config_s))) == NULL )
   {
    return SNS_DDF_ENOMEM;
   }

   SNS_OS_MEMCOPY(reg_group_data_ptr, &state->stage_config, sizeof(sns_dd_ad7146_config_s));

   reg_group_ptr->group_data = reg_group_data_ptr;
   reg_group_ptr->size = sizeof(sns_dd_ad7146_config_s);
   *(sns_ddf_registry_group_s**)value = reg_group_ptr;
  
   *num_elems = 1;
   return SNS_DDF_SUCCESS;

  default:
    return SNS_DDF_EINVALID_ATTR;
  }
}

/*===========================================================================*
 * @brief sns_dd_ad7146_change_power_mode: Change device power state.
 *
 * @param[in] state    Handle to driver instance.
 *
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_change_power_mode(
    sns_ddf_handle_t  handle,
    int               power_mode)
{
  /* cont mode disabled, manual mode enabled, CDR =0, TIM = 0 */
  sns_dd_ad7146_state_t* state = handle;
  uint16_t reg_data;
  sns_ddf_status_e ret;

  AD7146_MSG_1(HIGH, "sns_dd_ad7146_change_power_mode - enter mode=%d",
      power_mode);

  ret = sns_dd_ad7146_read_register(handle, AD7146_PWR_CONTROL_REG, &reg_data);

  AD7146_MSG_1(MED, "AD7146_PWR_CONTROL_REG = %X", reg_data);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);
    return ret;
  }

  if (power_mode == SNS_DDF_POWERSTATE_LOWPOWER)
  {
    reg_data |= 0x0003; // Shutdown the chip
  }
  else if (power_mode == SNS_DDF_POWERSTATE_ACTIVE)
  {
    reg_data &= 0xFFFC; // return to normal operation
  }
  else
    return SNS_DDF_EINVALID_PARAM;

  AD7146_MSG_1(MED, "write register = %X", reg_data);

  ret = sns_dd_ad7146_write_register(handle, AD7146_PWR_CONTROL_REG, reg_data);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_change_power_mode ERROR =%X", ret);
    return ret;
  }
  else
  {
    state->powerstate = power_mode;
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_enable_interrupt: Enable interrupts for configured stages
 *
 * @param[in] handle      Device handle.
 *
 *  @return SNS_DDF_SUCCESS if success else error code.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_enable_interrupt(sns_ddf_handle_t handle)
{
  sns_dd_ad7146_state_t* state = handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  // Set the lowest @number_of_stages bits to 1
  uint16_t int_enable = 1;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_enable_interrupt - enter");

  // Register the signal
  ret = sns_ddf_signal_register(state->interrupt_gpio, handle,
      &sns_dd_sar_ad7146_fn_list, SNS_DDF_SIGNAL_IRQ_FALLING);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_ddf_signal_register - ERROR=%d", ret);
    return ret;
  }

  int_enable = (int_enable << state->stage_config.number_of_stages) - 1;

  // Enable COMPLETE INT for the necessary stages for first few cycles.
  // This is to ensure first data notification is sent to SMGR when streaming
  // is started. Driver will then use HIGH INT for the necessary stages.
  ret = sns_dd_ad7146_write_register(handle,
      AD7146_STAGE_COMPLETE_INT_ENABLE_REG, int_enable);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_enable_interrupt - ERROR=%d", ret);
    return ret;
  }

#ifdef AD7146_DEBUG
  sns_dd_ad7146_read_stage_0_reg(handle);
  sns_dd_ad7146_read_stage_1_reg(handle);
  sns_dd_ad7146_read_stage_2_reg(handle);
#endif // AD7146_DEBUG

  sns_dd_ad7146_read_bank_1_reg(handle);
  state->is_data_requested = true;
  state->first_indication_count = 0;
  state->first_indication_sent = false;

  return ret;

}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_disable_interrupt: Disable all interrupts
 *
 * @param[in] handle      Device handle.
 *
 *  @return SNS_DDF_SUCCESS if success else error code.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_disable_interrupt(sns_ddf_handle_t handle)
{
  sns_dd_ad7146_state_t* state = handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_disable_interrupt - enter");

  // Disable all high interrupts
  ret = sns_dd_ad7146_write_register(handle, AD7146_STAGE_HIGH_INT_ENABLE_REG,
      0x0000);
  ret = sns_dd_ad7146_write_register(handle,
      AD7146_STAGE_COMPLETE_INT_ENABLE_REG, 0x0000);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_1(ERROR, "sns_dd_ad7146_disable_interrupt - ERROR=%d", ret);
    return ret;
  }

#ifdef AD7146_DEBUG
  sns_dd_ad7146_read_stage_0_reg(handle);
  sns_dd_ad7146_read_stage_1_reg(handle);
  sns_dd_ad7146_read_stage_2_reg(handle);
#endif // AD7146_DEBUG

  sns_dd_ad7146_read_bank_1_reg(handle);
  state->is_data_requested = false;

  // De register signal
  return sns_ddf_signal_deregister(state->interrupt_gpio);
}

static sns_ddf_status_e sns_dd_ad7146_enable_sched_data(
    sns_ddf_handle_t handle,
    sns_ddf_sensor_e sensor,
    bool enable)
{
  AD7146_MSG_1(HIGH, "sns_dd_ad7146_enable_sched_data enable=%d- enter",
      enable);

  if (enable)
  {
    return sns_dd_ad7146_enable_interrupt(handle);
  }
  else
  {
    return sns_dd_ad7146_disable_interrupt(handle);
  }

  return SNS_DDF_EINVALID_PARAM;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_notify_data: Handle data notifications in DRI mode.
 *                                   Data is sent for stages in the
 *                                   format:
 *                              Axis 0: Stage/Electrode (no units, in q16 format)
 *                              Axis 1: CDC raw counts (no units)
 *                              Axis 2: Zero (unused)
 *
 * @param[in] handle      Deivce handle.
 * @param[in] stage_num   Stage to return data for.
 * @param[in] timestamp   Timestamp for data notifs.
 *
 *  @return none.
 */
/*===========================================================================*/
static void sns_dd_ad7146_notify_data(
    sns_ddf_handle_t  handle,
    uint32_t          stage_num,
    sns_ddf_time_t    timestamp)
{
  sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *) handle;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t reg_data = 0;

  AD7146_MSG_1(HIGH, "sns_dd_ad7146_notify_data sensor=%d", stage_num);

  if (state->first_indication_count > NUM_CONVERSION_CYCLE_TO_SKIP)
  {
    if (!state->first_indication_sent)
    {
      state->first_indication_sent = true;
      AD7146_MSG_1(HIGH, "first_indication_count=%d", state->first_indication_count);
      ret = sns_dd_ad7146_write_register(handle, AD7146_STAGE_COMPLETE_INT_ENABLE_REG, 0x0000);

      // Set the lowest @number_of_stages bits to 1
      uint16_t int_enable = 1;
      int_enable = (int_enable << state->stage_config.number_of_stages) - 1;

      // Enable HIGH INT for the necessary stages.
      ret = sns_dd_ad7146_write_register(handle, AD7146_STAGE_HIGH_INT_ENABLE_REG, int_enable);
    }
  }
  else
  {
    state->first_indication_count++;
    return;
  }

  // Validate "stage_num"
  if (stage_num > SAR_AD7146_MAX_NUM_OF_STAGES)
  {
    AD7146_MSG_1(ERROR, "Invalid stage (%d) given for notify_data... exiting", stage_num);
    return;
  }

  state->s_data->timestamp = timestamp;
  state->s_data->num_samples = SAR_AD7146_NUM_OF_AXES;
  state->s_data->sensor = SNS_DDF_SENSOR_SAR;

  // If stage_num is 0, then that signifies an "Inactive" event
  if (stage_num == 0)
  {
    state->s_data->status = SNS_DDF_SUCCESS;
    state->s_data->samples[0].sample = 0; // "Inactive" event
    state->s_data->samples[0].status = SNS_DDF_SUCCESS;
    state->s_data->samples[1].sample = 0; // No CDC value to read
    state->s_data->samples[1].status = SNS_DDF_SUCCESS;
    state->s_data->samples[2].sample = 0; // UNUSED
    state->s_data->samples[2].status = SNS_DDF_SUCCESS;
  }
  else
  {
    state->s_data->status = SNS_DDF_SUCCESS;
    state->s_data->samples[0].sample = FX_CONV_Q16(stage_num,0);
    state->s_data->samples[0].status = SNS_DDF_SUCCESS;
    ret = sns_dd_ad7146_read_register(handle,
        AD7146_CDC_RESULT_Sx_REG_BASE + stage_num - 1, &reg_data);
    state->s_data->samples[1].sample = reg_data;
    state->s_data->samples[1].status = SNS_DDF_SUCCESS;
    state->s_data->samples[2].sample = 0; // UNUSED
    state->s_data->samples[2].status = SNS_DDF_SUCCESS;
  }

  state->s_data->end_timestamp = sns_ddf_get_timestamp();

  if (ret != SNS_DDF_SUCCESS)
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);

  ret = sns_ddf_smgr_notify_data(state->smgr_handle, state->s_data, 1);

  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_2(ERROR, "%d  - data notify error=%d", __LINE__, ret);
  }
  else
  {
    /* Log data */
    sns_dd_ad7146_log_data(state, stage_num, (uint32_t) reg_data);
  }
}

/*===========================================================================

 FUNCTION:   sns_dd_ad7146_handle_irq

 ===========================================================================*/
/*!
 @brief AD7146 interrupt handler

 @detail
 This function will be called within the context of the Interrupt
 Service Task (IST), *not* the ISR.

 @param[in] dd_handle  Handle to a driver instance.
 @param[in] irq        The IRQ representing the interrupt that occured.

 @return None
 */
/*=========================================================================*/
static void sns_dd_ad7146_handle_irq(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
  if (handle == NULL )
  {
    AD7146_MSG_0(ERROR, "sns_dd_ad7146_handle_irq - null handle");
    return;
  }

  sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *) handle;
  sns_ddf_status_e ret;
  uint16_t reg_data;

  AD7146_MSG_1(HIGH, "sns_dd_ad7146_handle_irq - enter gpio_num=%d", gpio_num);

  // read LOW INT status - value is not used but reading the reading the register to make
  // sure it clears
  ret = sns_dd_ad7146_read_register(handle, AD7146_STAGE_LOW_INT_STATUS_REG,
      &reg_data);

  AD7146_MSG_1(MED, "AD7146_STAGE_LOW_INT_STATUS_REG = %X", reg_data);

  if (ret != SNS_DDF_SUCCESS)
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);

  // read COMPLETE INT status - value is not used but reading the reading the register to make
  // sure it clears
  ret = sns_dd_ad7146_read_register(handle,
      AD7146_STAGE_COMPLETE_INT_STATUS_REG, &reg_data);

  AD7146_MSG_1(MED, "AD7146_STAGE_COMPLETE_INT_STATUS_REG = %X", reg_data);

  if (ret != SNS_DDF_SUCCESS)
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);

  reg_data = 0;

  // read HIGH INT status - value is used to determine data notifs
  ret = sns_dd_ad7146_read_register(handle, AD7146_STAGE_HIGH_INT_STATUS_REG,
      &reg_data);

  AD7146_MSG_1(MED, "AD7146_STAGE_HIGH_INT_STATUS_REG = %X", reg_data);

  if (ret != SNS_DDF_SUCCESS)
    AD7146_MSG_1(ERROR, "I2C Read error=%d", ret);

  if (state->is_data_requested)
  {
    if (reg_data == 0x0000)
    {
      // This is the case when sensor changes state to inactive
      sns_dd_ad7146_notify_data(handle, reg_data, timestamp);
    }
    else
    {
      // Check the stages for the interrupt
      uint32_t i;

      for (i = 0; i < state->stage_config.number_of_stages; i++)
      {
        if (reg_data & (0x0001 << i))
        {
          sns_dd_ad7146_notify_data(handle, i + 1, timestamp);
        }
      }
    }
  }

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_handle_irq - end");
}


/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_handle_timer: Handle data notifications in polling mode.
 *                                   Data is sent for all configured stages in the
 *                                   format:
 *                              Axis 0: Stage/Electrode (no units, in q16 format)
 *                              Axis 1: CDC raw counts (no units)
 *                              Axis 2: AFE in counts (no units)
 *
 * @param[in] dd_handle      Deivce handle.
 * @param[in] arg            Argument.
 *
 *  @return none.
 */
/*===========================================================================*/
static void sns_dd_ad7146_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
)
{
  sns_dd_ad7146_state_t *state = (sns_dd_ad7146_state_t *)dd_handle;
  sns_ddf_status_e      ret = SNS_DDF_SUCCESS;
  uint16_t cdc_value = 0;
  uint16_t afe_value = 0;
  uint16_t stage;
  int i;
  bool afe_changed = false;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_handle_timer - enter");

  if(state == NULL)
  {
    return;
  }

  // handle calibration request if enabled
  if(state->run_calibration == true)
  {
    // Loop through all stages and find appropriate AFE for each stage
    for(i = 0; i < state->stage_config.number_of_stages; i++)
    {
      ret = sns_dd_ad7146_config_afe(dd_handle, i, &afe_changed);
      if( ret != SNS_DDF_SUCCESS)
      {
        AD7146_MSG_2(ERROR,"sns_dd_ad7146_config_afe error=%d, stage=%d", ret, i); 
        sns_ddf_smgr_notify_test_complete(state->smgr_handle, SNS_DDF_SENSOR_SAR, SNS_DDF_EFAIL, 1);   
        state->run_calibration = false;
        return;
      }

    }  

    // send notification to SMGR that there is change in registry
    sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_SAR, 
                             SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);

    sns_ddf_smgr_notify_test_complete(state->smgr_handle, SNS_DDF_SENSOR_SAR, SNS_DDF_SUCCESS, 0);      
    state->run_calibration = false;
    AD7146_MSG_1(HIGH,"self-test error=%d", ret);
    return;
  }
 
  state->is_data_requested = false;

  state->s_data->sensor = SNS_DDF_SENSOR_SAR;
  state->s_data->status = SNS_DDF_SUCCESS;
  state->s_data->end_timestamp = sns_ddf_get_timestamp();
  state->s_data->timestamp = state->timestamp;

  for(stage = 0; stage < state->stage_config.number_of_stages; stage++)
  {

    ret = sns_dd_ad7146_read_register(dd_handle,
        AD7146_CDC_RESULT_Sx_REG_BASE + stage, &cdc_value);
    if (ret != SNS_DDF_SUCCESS)
    {
       return;
    }

    ret = sns_dd_ad7146_read_register(dd_handle,
        AD7146_STAGEx_AFE_OFFSET_REG_BASE + (stage * AD7146_STAGE_INCREMENT),
        &afe_value);
    if (ret != SNS_DDF_SUCCESS)
    {
       return;
    }

    AD7146_MSG_3(MED,
        "AD7146_CDC_RESULT_Sx_REG = 0x%x  stage %d afe value 0x%x", cdc_value,
        stage, afe_value);

    /**
     * Set the sample data
     *
     * Axis 0: Stage/Electrode (no units, in q16 format)
     * Axis 1: CDC raw counts (no units)
     * Axis 2: AFE in counts (no units)
     */
    state->s_data->samples[0 + (stage*3)].sample = FX_CONV_Q16((stage + 1),0);
    state->s_data->samples[0 + (stage*3)].status = SNS_DDF_SUCCESS;
    state->s_data->samples[1 + (stage*3)].sample = cdc_value;
    state->s_data->samples[1 + (stage*3)].status = SNS_DDF_SUCCESS;
    state->s_data->samples[2 + (stage*3)].sample = afe_value;
    state->s_data->samples[2 + (stage*3)].status = SNS_DDF_SUCCESS;
  }

  state->s_data->num_samples = SAR_AD7146_NUM_OF_AXES * state->stage_config.number_of_stages;

  ret = sns_ddf_smgr_notify_data(state->smgr_handle, state->s_data, 1);
  if (ret != SNS_DDF_SUCCESS)
  {
    AD7146_MSG_2(ERROR, "%d  - data notify error=%d", __LINE__, ret);
  }
  else
  {
    /* Log data */
    sns_dd_ad7146_log_data(state, (stage + 1), (uint32_t) cdc_value);
  }

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_handle_timer - end");
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_probe: Determines whether sensor is present based on 
 *                             ID register.
 *
 * @param[in] device_info    Deivce I2C info.
 * @param[in] memhandler     Memory handler.
 * @param[out] num_sensors   Number of sensor types supported by driver.
 * @param[out] sensors       List sensor types supported by driver.
 *
 *  @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_probe(
    sns_ddf_device_access_s*  device_info,
    sns_ddf_memhandler_s*     memhandler,
    uint32_t*                 num_sensors,
    sns_ddf_sensor_e**        sensors)
{
  sns_dd_ad7146_state_t* state;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint16_t reg_data;

  AD7146_MSG_0(HIGH, "sns_dd_ad7146_probe - enter");

  /* Basic sanity check*/
  if (device_info == NULL || memhandler == NULL || sensors == NULL ||
      num_sensors == NULL )
    return SNS_DDF_EINVALID_PARAM;

  if (sns_ddf_malloc((void **) &state, sizeof(*state)) != SNS_DDF_SUCCESS)
    return SNS_DDF_ENOMEM;

  memset(state, 0, sizeof(*state));

  /* open I2C port */
  ret = sns_ddf_open_port(&state->port_handle, &device_info->port_config);

  if (ret != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(state);
    return ret;
  }

  ret = sns_dd_ad7146_read_register((sns_ddf_handle_t) state,
      AD7146_DEVICE_ID_REG, &reg_data);

  AD7146_MSG_1(MED, "AD7146_DEVICE_ID_REG = %X", reg_data);

  if (ret != SNS_DDF_SUCCESS)
  {
    sns_ddf_close_port(state->port_handle);
    sns_ddf_mfree(state);
    return ret;
  }

  // bits 15:4 is ID registers = 0x149
  reg_data &= 0xFFF0;

  if (reg_data != 0x1490)
  {
    sns_ddf_close_port(state->port_handle);
    sns_ddf_mfree(state);
    return SNS_DDF_EDEVICE;
  }

  *num_sensors = 2;
  *sensors = (sns_ddf_sensor_e *) sns_dd_ad7146_sensor_types;
  sns_ddf_close_port(state->port_handle);
  sns_ddf_mfree(state);

  return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_config_afe: Performs sensor characterization by
 *                                 determining approproate AFE per stage.
 *
 * @param[in] dd_handle      Handle to driver instance.
 * @param[in] stage          ADC stage to calibrate.
 * @param[out] afe_changed   True if AFE value changed wrt to register, else false.
 *
 *  @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_config_afe(sns_ddf_handle_t dd_handle, 
                                           uint32_t stage, bool *afe_changed)
{
 sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *)dd_handle;
	sns_ddf_status_e	ret = SNS_DDF_SUCCESS;
 uint16_t afe = 0;
 uint16_t cdc = 0;

 AD7146_MSG_1(ERROR,"sns_dd_ad7146_config_afe stage=%d", stage);

 if((stage >= SAR_AD7146_MAX_NUM_OF_STAGES) || (afe_changed == NULL) || (dd_handle == NULL))
 {
   return SNS_DDF_EINVALID_PARAM;
 }
 
 *afe_changed = false;

 do
 {
   if(afe == AD7146_MAX_AFE_VALUE)
   {
      // Max value of AFE is 0x3F00. If this algo reaches this value then something is not right.
      // return an error.
      return SNS_DDF_EFAIL;
   }

   ret = sns_dd_ad7146_write_register(dd_handle, 
                                      (AD7146_STAGEx_AFE_OFFSET_REG_BASE+(stage*AD7146_STAGE_INCREMENT)), 
                                      afe);
   if( ret != SNS_DDF_SUCCESS)
   {
     AD7146_MSG_2(ERROR,"sns_dd_ad7146_write_register error=%d, stage=%d", ret, stage); 
     return ret;
   }

   // delay to allow change in AFE to reflect on CDC 
   sns_ddf_delay(AD7146_STAGE_CONVERSION_DELAY_US*(stage+1)*2);

   ret = sns_dd_ad7146_read_register(dd_handle, (AD7146_CDC_RESULT_Sx_REG_BASE+stage), &cdc);
   if( ret != SNS_DDF_SUCCESS)
   {
     AD7146_MSG_2(ERROR,"sns_dd_ad7146_read_register error=%d, stage=%d", ret, stage); 
     return ret;
   }

    AD7146_MSG_2(MED,"sns_dd_ad7146_read_register afe=%02X, cdc=%d", afe, cdc);

    afe += AD7146_AFE_INCREMENT;

  } while((cdc < AD7146_MID_SCALE_LOW_COUNT) || (cdc > AD7146_MID_SCALE_HIGH_COUNT)); 

 // If we came out of this loop that means we got a decent AFE value for 
 // CDC between AD7146_MID_SCALE_LOW_COUNT and AD7146_MID_SCALE_HIGH_COUNT
 
 // Get the AFE that was actually written to the sensor
 afe -= AD7146_AFE_INCREMENT;

 // if registry value is not the same as new AFE then update state variable
 if(afe != state->stage_config.stage_config_main_array[stage].afe_offset)
 {
   AD7146_MSG_2(HIGH,"old_afe=%02X, new_afe=%02X", 
              state->stage_config.stage_config_main_array[stage].afe_offset, afe);
   state->stage_config.stage_config_main_array[stage].afe_offset = afe;
   *afe_changed = true;    
 }

 return ret; 
}

/*===========================================================================*/
/*
 * @brief sns_dd_ad7146_self_test: Supported test types: SNS_DDF_TEST_OEM
 *                                                       SNS_DDF_TEST_SELF
 *
 * @param[in] dd_handle  Handle to driver instance.
 * @param[in] sensor     Sensor for which this attribute is to be set.
 * @param[in] test       Test Type.
 * @param[in] value      Value to set this attribute.
 *
 *  @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_ad7146_self_test( 
				sns_ddf_handle_t dd_handle, 
				sns_ddf_sensor_e sensor, 
				sns_ddf_test_e test,
				uint32_t* err)
{
 sns_dd_ad7146_state_t* state = (sns_dd_ad7146_state_t *)dd_handle;
 sns_ddf_status_e	ret = SNS_DDF_SUCCESS;
 uint16_t reg_data = 0;

 AD7146_MSG_2(HIGH,"sns_dd_ad7146_self_test test_type=%d sensor=%d", test, sensor);

 if((dd_handle == NULL) || (sensor != SNS_DDF_SENSOR_SAR))
 {
  AD7146_MSG_0(ERROR,"SNS_DDF_EINVALID_PARAM");
  return SNS_DDF_EINVALID_PARAM;
 }

 if(state->is_data_requested)
 {
  AD7146_MSG_1(ERROR,"SNS_DDF_EDEVICE_BUSY=%d", SNS_DDF_EDEVICE_BUSY); 
  return SNS_DDF_EDEVICE_BUSY;
 }

 if((test != SNS_DDF_TEST_OEM) && (test != SNS_DDF_TEST_SELF))
 {
  AD7146_MSG_1(ERROR,"SNS_DDF_EINVALID_TEST=%d", SNS_DDF_EINVALID_TEST); 
  return SNS_DDF_EINVALID_TEST;
 }

 // handle SNS_DDF_TEST_SELF first
 if(test == SNS_DDF_TEST_SELF)
 {
   ret = sns_dd_ad7146_read_register((sns_ddf_handle_t) state,
           AD7146_DEVICE_ID_REG, &reg_data);

   AD7146_MSG_1(MED, "AD7146_DEVICE_ID_REG = %X", reg_data);

   if (ret != SNS_DDF_SUCCESS)
   {
     return ret;
   }

   // bits 15:4 is ID registers = 0x149
   reg_data &= 0xFFF0;

   return ((reg_data != 0x1490) ? SNS_DDF_EFAIL : SNS_DDF_SUCCESS);

 }

  // else following code is for calibration test - SNS_DDF_TEST_OEM

	ret = sns_dd_ad7146_reset(dd_handle);
 if( ret != SNS_DDF_SUCCESS)
 {
  AD7146_MSG_1(ERROR,"sns_dd_ad7146_reset error=%d", ret); 
  return ret;
 }

 ret = sns_dd_ad7146_change_power_mode(dd_handle, SNS_DDF_POWERSTATE_ACTIVE);
 if( ret != SNS_DDF_SUCCESS)
 {
  AD7146_MSG_1(ERROR,"sns_dd_ad7146_change_power_mode error=%d", ret); 
  return ret;
 }

 sns_dd_ad7146_read_bank_1_reg(dd_handle); 

 state->run_calibration = true;

 // Start a 20ms timer to handle calibration - non-blocking test
 ret = sns_ddf_timer_start(state->sar_timer, SAR_AD7146_20_MS);

 if(ret != SNS_DDF_SUCCESS)
 {
    AD7146_MSG_0(ERROR,"self-test - timer start failed");
    return ret;
 }

 return SNS_DDF_PENDING;

}

sns_ddf_driver_if_s sns_dd_sar_ad7146_fn_list =
{
    .init = &sns_dd_ad7146_init,
    .get_data = &sns_dd_ad7146_get_data,
    .set_attrib = &sns_dd_ad7146_set_attrib,
    .get_attrib = &sns_dd_ad7146_get_attrib,
    .handle_timer = &sns_dd_ad7146_handle_timer, 
    .handle_irq = &sns_dd_ad7146_handle_irq,
    .reset = &sns_dd_ad7146_reset,
    .run_test = &sns_dd_ad7146_self_test, 
    .enable_sched_data = &sns_dd_ad7146_enable_sched_data,
    .probe = &sns_dd_ad7146_probe
};

