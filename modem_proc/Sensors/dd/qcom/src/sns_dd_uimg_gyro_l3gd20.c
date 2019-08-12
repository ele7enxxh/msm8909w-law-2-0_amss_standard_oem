/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : l3gd20_ddf_drv.c
 * Authors           : Jianjian Huo
 * Version           : V 2.2.3
 * Date              : 04/02/2014
 * Description       : L3GD20 gyroscope driver source file
 *
 ********************************************************************************
 * Copyright (c) 2014, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *`
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 * REVISON HISTORY
 *
 * VERSION | DATE          | DESCRIPTION
 * 2.2.4   | 07/20/204     | Disabled logging in uImage
 * 2.2.3   | 04/02/2014    | Increased idle to active time to 250ms, fixed one issue in set_datarate.
 * 2.2.2   | 03/31/2014    | In function enable_fifo, added BDU disabling code during enabling fifo.
 * 2.2.1   | 03/28/2014    | Enabled BDU when wmk is 1.
 *                           Fixed KW issue, bug in match_odr.
 * 2.2.0   | 03/03/2014    | Increased duration of unsettled ODR for gyro.
 * 2.1.9   | 02/12/2014    | Fixed negative timestamp issue between FIFO interrupt
 * 2.1.8   | 02/11/2014    | Reset state->last_timestamp only after dd reset or after fifo disabled.
 *                           Improved timing in handle_irq and trigger_fifo. 
 * 2.1.6   | 02/10/2014    | Fixed no fifo data sent problem in trigger_fifo when wmk is 1
 *                           Reorganized trigger_fifo function.
 * 2.1.3   | 12/10/2013    | Fixed a watermark bug in disable FIFO and INT code path. 
 * 2.1.2   | 12/02/2013    | Fixed last_timestamp bugs in send_fifo_data.
 * 2.1.0   | 11/20/2013    | Merged update from 1.6.5.
 *                           Marks invalid FIFO data produced before new ODR is settled. 
 * 2.0.0   | 06/14/2013    | Supported FIFO functionalities with improved timestamps.
 *                           Removed MD.
 * 1.6.5   | 11/15/2013    | Set delays based on ODR in set_datarate() and get_data().
 *                           Modified HW self test to use timer, added probe function.
 * 1.6.3   | 10/18/2013    | Increased ODR switch delay to 10ms, disabled LPF2.
 * 1.6.2   | 10/17/2013    | Removed compiling warnings.
 * 1.5.5   | 06/11/2013    | Updated turn-on time from 200ms to 500ms in SW self test.
 * 1.5.3   | 05/09/2013    | Added logic to exit from infinite while
 * 1.5.2   | 04/04/2013    | Modified bias calculation used mdps instead q16
 * 1.5.1   | 04/01/2013    | Added bias validation in OEM selftest
 * 1.5.0   | 03/15/2013    | Added support for OEM selftest
 * 1.4.2   | 03/07/2013    | Fixed the overflow condition with big LSB value.
 * 1.4.0   | 02/21/2013    | Added motion detection support.
 *                           Updated axes mapping.
 * 1.3.0   | 02/20/2013    | Combined HW and SW self tests.
 * 1.2.2   | 02/13/2013    | Updated SW inertial self test parameters.
 * 1.2.0   | 02/05/2013    | Added SW inertial self test.
 * 1.1.0   | 06/27/2012    | Commented out code which disable self test and sensor after self test is done.
 * 1.0.8   | 06/07/2012    | Fixed memset bugs in self test function,
 *                           Increased delay time of self test(after self test is enabled) from 20ms to 60ms.
 * 1.0.4   | 05/31/2012    | Corrected lps331ap_freqs array, it should be ODR/2.
 * 1.0.2   | 05/07/2012    | Updated self test.
 * 1.0.0   | 04/10/2012    | Added self test.
 * 0.9.4   | 03/26/2012    | Converted all floating point numbers to fixed point.
 * 0.9.3   | 01/02/2012    | Changed dynamic range unit from dps to rad/s.
 * 0.9.2   | 05/13/2011    | Fixed some bugs.
 * 0.9.1   | 05/13/2011    | Used a header file to configure axis and polarity.
 * 0.9.0   | 04/27/2011    | Created.
 *******************************************************************************/

/*******************************************************************************
* Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

/*=============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  11/15/2013  pn   Set delays based on ODR in set_datarate().
  10/28/2013  vh   Added Qualcomm Copyright & Synced with STM's version number
=============================================================================*/

#include "sns_dd_gyro_l3gd20.h"
#include <string.h>
#include <stdlib.h>

#if STM_L3GD20_LOGDATA
//Include files required to support gyro log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#endif

#define CONFIG_OEM
#define STM_L3GD20_NUM_READ_BYTES 6
#define AUTO_INCREMENT    0x80
#define BIAS_SPECIAL_ERROR_WAIT_TIME 100000 //100ms
#define PART_EXCEPTION 1

//#define L3GD20_DEBUG

#ifdef L3GD20_DEBUG
#define L3GD20_MSG_0(level,msg)             MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "L3GD20 - " msg)
#define L3GD20_MSG_1(level,msg,p1)          MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "L3GD20 - " msg,p1)
#define L3GD20_MSG_2(level,msg,p1,p2)       MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "L3GD20 - " msg,p1,p2)
#define L3GD20_MSG_3(level,msg,p1,p2,p3)    MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "L3GD20 - " msg,p1,p2,p3)
#define L3GD20_MSG_4(level,msg,p1,p2,p3,p4) MSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, "L3GD20 - " msg,p1,p2,p3,p4)
#else
#define L3GD20_MSG_0(level,msg)
#define L3GD20_MSG_1(level,msg,p1)
#define L3GD20_MSG_2(level,msg,p1,p2)
#define L3GD20_MSG_3(level,msg,p1,p2,p3)
#define L3GD20_MSG_4(level,msg,p1,p2,p3,p4)
#endif


extern sns_ddf_status_e sns_dd_gyr_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_gyr_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

extern sns_ddf_status_e sns_dd_gyr_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value);

extern sns_ddf_status_e sns_dd_gyr_reset(sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_gyr_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err);

extern sns_ddf_status_e sns_dd_gyr_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors );

extern volatile boolean EnableI2C;


/*===============================================================
*  FUNCTION PROTOTYPES
================================================================*/
static sns_ddf_status_e sns_dd_gyr_is_dataready(
    sns_ddf_handle_t dd_handle,
    bool *data_flag);

sns_ddf_status_e sns_dd_gyr_collect_testdata(
    sns_ddf_handle_t dd_handle,
    int16_t *data,
    uint32_t*   err,
    uint8_t num_samples);

static sns_ddf_status_e sns_dd_gyr_enable_fifo_int(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_gyr_disable_fifo_int(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_gyr_parse_and_send_fifo_data(
   sns_dd_gyr_l3gd20_state_t* state,
   sns_ddf_handle_t            smgr_handle,
   const uint8_t*              raw_buf_ptr,
   uint16_t                    num_samples);

static sns_ddf_status_e sns_dd_gyr_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    uint16_t num_samples,
    sns_ddf_time_t    start_time,
    sns_ddf_time_t    end_time);

static void sns_dd_gyr_handle_irq(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp);

static void sns_dd_gyr_log_data(sns_ddf_sensor_data_s *gyr_data_ptr);

static void sns_dd_gyr_log_fifo(sns_ddf_sensor_data_s *gyr_data_ptr);

static sns_ddf_status_e sns_dd_gyr_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data);

static void sns_dd_gyr_handle_timer(
    sns_ddf_handle_t dd_handle,
    void* arg);

#if 0
static sns_ddf_status_e sns_dd_lgd20_reg_dump(sns_dd_gyr_l3gd20_state_t *state);
#endif

static sns_ddf_status_e sns_dd_gyr_enable_sched(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable);

static sns_ddf_status_e sns_dd_gyr_trigger_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint16_t num_samples,
    bool trigger_now);


/*===============================================================
*  STRUCTURES
================================================================*/
/**
 * L3GD20 Gyroscope device driver interface.
 */
sns_ddf_driver_if_s sns_dd_gyr_if =
{
  &sns_dd_gyr_init,
  &sns_dd_gyr_get_data,
  &sns_dd_gyr_set_attr,
  &sns_dd_gyr_get_attr,
  &sns_dd_gyr_handle_timer,
  &sns_dd_gyr_handle_irq,
  &sns_dd_gyr_reset,
  &sns_dd_gyr_run_test,
  &sns_dd_gyr_enable_sched,
  &sns_dd_gyr_probe,
  &sns_dd_gyr_trigger_fifo
};


/*===============================================================
*  FUNCTION DEFINITIONS
================================================================*/
/**
 * @brief Reads FIFO_SRC register to extract the FIFO sample count
 *
 * @param[in]  state      The driver instance
 *
 * @return number of samples in FIFO
 */
static uint8_t sns_dd_gyr_get_fifo_sample_count(
  sns_dd_gyr_l3gd20_state_t* state)
{
  uint8_t fifo_count;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  sns_ddf_status_e status;
  sns_ddf_time_t cur_time, odr_interval;
  
  if(1 == state->fifo_wmk)
  {
    cur_time = sns_ddf_get_timestamp();
    odr_interval = sns_ddf_convert_usec_to_tick(1000000 / state->gyr_cur_rate);

    if((cur_time - state->last_timestamp) < odr_interval)
    {
      fifo_count = 0;
    }
    else
    {
      fifo_count = 1;
    } 
  }
  else
  {
    status  = sns_ddf_read_port(state->port_handle,
                                STM_L3GD20_FIFO_SRC,
                                &rw_buffer,
                                1,
                                &rw_bytes);
    if((status != SNS_DDF_SUCCESS) || (rw_bytes != 1))
    {
      fifo_count = 0;
    }
    else
    {
      fifo_count = rw_buffer & 0x1F;

      // if it's overflow, add one more sample.
      if(rw_buffer & 0x40)
      {
        fifo_count++;
      }
    }
  }
  return fifo_count;
}

/**
 * @brief Check ZYXDA in STATUS register, return data ready flag.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[out] data_flag      data ready flag to set.
 *                            true:  a new set of data is available.
 *                            false: a new set of data is not yet available.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_is_dataready(
    sns_ddf_handle_t dd_handle,
    bool *data_flag)
{
  sns_dd_gyr_l3gd20_state_t* state = dd_handle;
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  // read status register
  status = sns_ddf_read_port(
      state->port_handle,
      STM_L3GD20_STATUS_REG,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  if((rw_buffer&0x08) > 0)
    *data_flag = true;
  else
    *data_flag = false;

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Collects sample data for L3GD20 self test.
 *
 * @param[in]  dd_handle     Handle to a driver instance.
 * @param[out] data           Output data buffer to be filled in, should be set to 0 before called.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
#define STM_POOL_CNT    4
#define MAX_WAIT_COUNT  5  //total max wait time 50ms
sns_ddf_status_e sns_dd_gyr_collect_testdata(
    sns_ddf_handle_t dd_handle,
    int16_t *data,
    uint32_t*   err,
    uint8_t num_samples)
{
  sns_dd_gyr_l3gd20_state_t* state = dd_handle;
  sns_ddf_status_e status;
  uint8_t rw_buffer[STM_L3GD20_NUM_READ_BYTES] = {0,0,0,0,0,0};
  uint8_t rw_bytes = 0;
  bool data_rdy = false;
  uint8_t wait_time_count = 0;
  int32_t average_data[STM_L3GD20_NUM_AXES] = {0,0,0};
  int16_t pool_data[STM_POOL_CNT][STM_L3GD20_NUM_AXES];
  uint8_t idx_i;
  uint8_t idx_pool = 0, cnt_pool = 0, i_pool = 0, j_pool = 0;

  //Collect data
  for(idx_i=0;idx_i<num_samples;idx_i++)
  {
    wait_time_count = 0;
    do{
      status = sns_dd_gyr_is_dataready(dd_handle,&data_rdy);
      if(status != SNS_DDF_SUCCESS){
        return status;
      }
      if(data_rdy == false) {
        sns_ddf_delay(10000);
        if(++wait_time_count > MAX_WAIT_COUNT){
          L3GD20_MSG_3(HIGH, "Data not ready for 50ms ", 0, 0, 0);
          *err = AXIS_SPECIAL_ERROR;
          return SNS_DDF_EDEVICE;
        }
      }
    }while(data_rdy == false);

    status = sns_ddf_read_port(
        state->port_handle,
        AUTO_INCREMENT | STM_L3GD20_OUT_X_L,
        rw_buffer,
        STM_L3GD20_NUM_READ_BYTES,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != STM_L3GD20_NUM_READ_BYTES)
      return SNS_DDF_EBUS;

    if(0==idx_i)
      continue;

    pool_data[idx_pool][0] = (int16_t) (((rw_buffer[1]) << 8) | rw_buffer[0]);
    pool_data[idx_pool][1] = (int16_t) (((rw_buffer[3]) << 8) | rw_buffer[2]);
    pool_data[idx_pool][2] = (int16_t) (((rw_buffer[5]) << 8) | rw_buffer[4]);
    cnt_pool++;
    idx_pool++;
    idx_pool %= STM_POOL_CNT;
    if (cnt_pool > STM_POOL_CNT){
      // check consecutive data values are the same
      for (i_pool=1,j_pool=0;i_pool<STM_POOL_CNT;i_pool++){
        if ((pool_data[0][0] == pool_data[i_pool][0])
            && (pool_data[0][1] == pool_data[i_pool][1])
            && (pool_data[0][2] == pool_data[i_pool][2]))
          j_pool++;
      }
      if (j_pool > 0){
        // fixed values are detected
        *err = (BIAS_SPECIAL_ERROR << 8);
        return SNS_DDF_EFAIL;
      }
    }

    average_data[0] += (int16_t) (((rw_buffer[1]) << 8) | rw_buffer[0]);
    average_data[1] += (int16_t) (((rw_buffer[3]) << 8) | rw_buffer[2]);
    average_data[2] += (int16_t) (((rw_buffer[5]) << 8) | rw_buffer[4]);
  }

  for(idx_i=0;idx_i<STM_L3GD20_NUM_AXES;idx_i++)
    data[idx_i] = average_data[idx_i] / (num_samples - 1); //ignoring first sample

  return SNS_DDF_SUCCESS;
}


/**
 * @brief To enable L3GD20 FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_enable_fifo_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_gyr_l3gd20_state_t* state = dd_handle;
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  if(false == state->fifo_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_enabled && state->fifo_int_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_wmk > 1)
  {
    // Configure gyro FIFO control registers
    rw_buffer = 0x0
      | (0<<7)					 // enable INT on INT1 pin
      | (0<<6)					 // Boot status available on INT1
      | (0<<5)					 // Interrupt active configuration on INT1.
      | (0<<4)					 // Push-Pull / Open drain
      | (0<<3)					 // Data Ready on DRDY/INT2.
      | (1<<2)					 // FIFO Watermark interrupt on DRDY/INT2.
      | (1<<1)					 // FIFO Overrun interrupt on DRDY/INT2.
      | 0;					 // FIFO Empty interrupt on DRDY/INT2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_L3GD20_CTRL_REG3,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;
  }else  if(1 == state->fifo_wmk)
    //special case, watermark is 1, enable DRI instead of FIFO.
  {
    // Configure gyro DRY
    rw_buffer = 0x0
      | (0<<7)					 // enable INT on INT1 pin
      | (0<<6)					 // Boot status available on INT1
      | (0<<5)					 // Interrupt active configuration on INT1.
      | (0<<4)					 // Push-Pull / Open drain
      | (1<<3)					 // Data Ready on DRDY/INT2.
      | (0<<2)					 // FIFO Watermark interrupt on DRDY/INT2.
      | (0<<1)					 // FIFO Overrun interrupt on DRDY/INT2.
      | 0;					 // FIFO Empty interrupt on DRDY/INT2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_L3GD20_CTRL_REG3,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

  }else
  {
    return SNS_DDF_EINVALID_PARAM;
  }
#if 0
  status =  sns_ddf_signal_register(
      state->gpio_num,
      state,
      &sns_dd_gyr_if,
      SNS_DDF_SIGNAL_IRQ_RISING);

  if(status != SNS_DDF_SUCCESS)
    return status;
#endif
  state->fifo_int_enabled = true;

  L3GD20_MSG_2(HIGH, "fifo int enabled: gpio_num=%u, status=%u",
      state->gpio_num, status);

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To disable L3GD20 FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_disable_fifo_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_gyr_l3gd20_state_t* state = dd_handle;
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  if(false == state->fifo_int_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_wmk > 1)
  {
    // Configure gyro FIFO control registers
    rw_buffer = 0x0
      | (0<<7)					 // enable INT on INT1 pin
      | (0<<6)					 // Boot status available on INT1
      | (0<<5)					 // Interrupt active configuration on INT1.
      | (0<<4)					 // Push-Pull / Open drain
      | (0<<3)					 // Data Ready on DRDY/INT2.
      | (0<<2)					 // FIFO Watermark interrupt on DRDY/INT2.
      | (0<<1)					 // FIFO Overrun interrupt on DRDY/INT2.
      | 0;					 // FIFO Empty interrupt on DRDY/INT2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_L3GD20_CTRL_REG3,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;
  }
  else if(1 == state->fifo_wmk)
  {
    //special case, if watermark is 1, clear gyro DRI INT.
    rw_buffer = 0x0
      | (0<<7)					 // enable INT on INT1 pin
      | (0<<6)					 // Boot status available on INT1
      | (0<<5)					 // Interrupt active configuration on INT1.
      | (0<<4)					 // Push-Pull / Open drain
      | (0<<3)					 // Data Ready on DRDY/INT2.
      | (0<<2)					 // FIFO Watermark interrupt on DRDY/INT2.
      | (0<<1)					 // FIFO Overrun interrupt on DRDY/INT2.
      | 0;					 // FIFO Empty interrupt on DRDY/INT2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_L3GD20_CTRL_REG3,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

  }else
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  state->fifo_int_enabled = false;
  
  //status = sns_ddf_signal_deregister(state->gpio_num);
  //if(SNS_DDF_SUCCESS != status)
    //return status;

  return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION:   sns_dd_gyr_parse_and_send_fifo_data
===========================================================================*/
/*!
  @brief Parses FIFO data and sends to SMGR
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyr_parse_and_send_fifo_data(
   sns_dd_gyr_l3gd20_state_t* state,
   sns_ddf_handle_t            smgr_handle,
   const uint8_t*              raw_buf_ptr,
   uint16_t                    num_samples
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t idx;
  // sensor coordinate x,y,z axis raw data sample
  static int16_t hw_d[STM_L3GD20_NUM_AXES];
  // SAE coordinate x,y,z axis data sample
  static q16_t sae_d[STM_L3GD20_NUM_AXES];

  for(idx=0; idx<num_samples; idx++)
  {
    //convert the raw data in read_buffer to X/Y/Z axis sensor data
    hw_d[0] = (int16_t) (((raw_buf_ptr[1 + idx*6]) << 8) |
        raw_buf_ptr[0 + idx*6]);
    hw_d[1] = (int16_t) (((raw_buf_ptr[3 + idx*6]) << 8) |
        raw_buf_ptr[2 + idx*6]);
    hw_d[2] = (int16_t) (((raw_buf_ptr[5 + idx*6]) << 8) |
        raw_buf_ptr[4 + idx*6]);

    //adjust sensor data per current sensitivity, and convert to Q16
    sae_d[0] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)hw_d[0],0), FX_CONV_Q16((int32_t)1000,0)),
        state->gyr_sstvt);
    sae_d[1] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)hw_d[1],0), FX_CONV_Q16((int32_t)1000,0)),
        state->gyr_sstvt);
    sae_d[2] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)hw_d[2],0), FX_CONV_Q16((int32_t)1000,0)),
        state->gyr_sstvt);

    //map the sensor data to the phone's coordinates
    sns_ddf_map_axes(&state->axes_map, sae_d);

    //Fill out sensor output data and status
    state->fifo_data.samples[0 + idx*3].sample = sae_d[0];
    state->fifo_data.samples[0 + idx*3].status = SNS_DDF_SUCCESS;
    state->fifo_data.samples[1 + idx*3].sample = sae_d[1];
    state->fifo_data.samples[1 + idx*3].status = SNS_DDF_SUCCESS;
    state->fifo_data.samples[2 + idx*3].sample = sae_d[2];
    state->fifo_data.samples[2 + idx*3].status = SNS_DDF_SUCCESS;

  }

  state->fifo_data.num_samples = STM_L3GD20_NUM_AXES * num_samples;

  // send fifo data to SMGR
  status = sns_ddf_smgr_notify_data(
      state->smgr_hndl,
      &state->fifo_data,
      1);
  if(status != SNS_DDF_SUCCESS)
    return status;

  L3GD20_MSG_1(HIGH, "parse_and_send: sent data successfully. fifo_data.num_samples=%u",
      state->fifo_data.num_samples);

  L3GD20_MSG_1(HIGH, "parse_and_send: end time=%u", sns_ddf_get_timestamp());

#if STM_L3GD20_LOGDATA
  sns_dd_gyr_log_fifo(&state->fifo_data);
#endif

  return SNS_DDF_SUCCESS;
}


/**
 * @brief To send FIFO data to SMGR, numbers of sample up to watermark level.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  num_samples Num of data samples in FIFO to read.
 * @param[in]  timestamp      The end timestamp.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    uint16_t num_samples,
    sns_ddf_time_t    start_time,
    sns_ddf_time_t    end_time)
{
  sns_dd_gyr_l3gd20_state_t* state = dd_handle;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t read_count = 0;
  uint16_t num_invalid_samples;
  uint8_t* raw_buf_ptr = state->fifo_raw_buffer;

  // read all FIFO data up to num_samples.
  status = sns_ddf_read_port(
      state->port_handle,
      AUTO_INCREMENT | STM_L3GD20_OUT_X_L,
      state->fifo_raw_buffer,
      STM_L3GD20_NUM_READ_BYTES * num_samples,
      &read_count);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(read_count != STM_L3GD20_NUM_READ_BYTES * num_samples)
  {
    L3GD20_MSG_2(HIGH, "send_fifo_data: raw data reading fails, num_samples_to_read=%u, actual_read_bytes=%u",
        num_samples, read_count);
    return SNS_DDF_EBUS;
  }

  // fill out fifo_data structure
  state->fifo_data.sensor = SNS_DDF_SENSOR_GYRO;
  state->fifo_data.end_timestamp = end_time;
  state->fifo_data.timestamp = start_time;

  L3GD20_MSG_2(HIGH, "send_fifo_data: start_timestamp=%u, end_timestamp=%u",
      state->fifo_data.timestamp, state->fifo_data.end_timestamp);

  if ( (int32_t)(state->odr_settled_ts - state->fifo_data.timestamp) <= 0 )
  {
    /* all samples in FIFO produced after ODR change settled */
    num_invalid_samples = 0;
  }
  else if ( (int32_t)(state->odr_settled_ts - state->fifo_data.end_timestamp) > 0 )
  {
    /* all samples in FIFO produced before ODR change settled */
    num_invalid_samples = num_samples;
  }
  else /* some samples in FIFO produced before ODR change settled */
  {
    //last_timestamp could be zero or not reliable. Use input parameters of this function.
    uint32_t time_interval = (end_time - start_time)/(num_samples-1); 
    num_invalid_samples = 
      ((state->odr_settled_ts - state->fifo_data.timestamp) / time_interval) + 1;
    state->fifo_data.end_timestamp = 
      start_time + (time_interval * (num_invalid_samples-1));
    //set temporary last_timestamp within this function.
    state->last_timestamp = state->fifo_data.end_timestamp;
  }

  if ( num_invalid_samples > 0 )
  {
    state->fifo_data.status      = SNS_DDF_EINVALID_DATA;
    state->fifo_data.num_samples = STM_L3GD20_NUM_AXES * num_invalid_samples;
    status = sns_dd_gyr_parse_and_send_fifo_data(
       state, state->smgr_hndl, raw_buf_ptr, num_invalid_samples);
    MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
          "send_fifo_data: #samples=%d #invalid=%d end_ts=%u", 
          num_samples, num_invalid_samples, state->fifo_data.end_timestamp);

    /* prepares to send remaining samples */
    num_samples -= num_invalid_samples;
    if ( num_samples > 0 )
    {
      raw_buf_ptr = &state->fifo_raw_buffer[state->fifo_data.num_samples<<1];
      state->fifo_data.end_timestamp = end_time;
      state->fifo_data.timestamp     = state->last_timestamp +
        (end_time - state->last_timestamp)/num_samples;
      MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
            "send_fifo_data: #valid=%d ts=%u end_ts=%u", 
            num_samples, state->fifo_data.timestamp, state->fifo_data.end_timestamp);
    }
  }
  if ( num_samples > 0 )
  {
    state->fifo_data.status = SNS_DDF_SUCCESS;
    state->fifo_data.num_samples = STM_L3GD20_NUM_AXES * num_samples;
    status |= sns_dd_gyr_parse_and_send_fifo_data(
       state, state->smgr_hndl, raw_buf_ptr, num_samples);
  }

  //set final last_timestamp value.
  state->last_timestamp = end_time;

  return status;
}


/**
 * @brief Called in response to an motion detection interrupt for this driver.
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in]  gpio_num     GPIO number that generated the interrupt.
 * @param[in]  timestamp    Time at which ISR is called
 *
 * @return none
 * */
static void sns_dd_gyr_handle_irq(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
  sns_dd_gyr_l3gd20_state_t* state = handle;
  sns_ddf_status_e status;
  uint8_t rw_buffer[2] = {0, 0};
  uint8_t rw_bytes = 0;
  uint8_t total_in_fifo = 0;
  //flag for the while loop to read out any new data samples during previous INT handling.
  bool run_again = true;
  //how many times fifo_read happens
  uint8_t fifo_run_cnt = 0;
  sns_ddf_time_t end_time=0, cal_end=0, start_time=0, cal_odr=0, est_odr=0;

  if(handle==NULL)
  {
    L3GD20_MSG_0(HIGH, "handle_irq: Received Null Pointer.");
    return;
  }
  L3GD20_MSG_1(HIGH, "handle_irq: CPU IRQ time=%u", timestamp);
  L3GD20_MSG_1(HIGH, "handle_irq: DD ISR start time=%u", sns_ddf_get_timestamp());

  if (gpio_num != state->gpio_num)
    return;

  est_odr = TICKS_PER_SEC/state->gyr_cur_rate;

  while(run_again)
  {
    run_again = false;

    // verify source of interrupts by reading INT1_SRC.
    status = sns_ddf_read_port(
        state->port_handle,
        STM_L3GD20_STATUS_REG,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return;
    if(rw_bytes != 1)
      return;

    // verify source of interrupts by reading FIFO_SRC.
    status = sns_ddf_read_port(
        state->port_handle,
        STM_L3GD20_FIFO_SRC,
        rw_buffer+1,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return;
    if(rw_bytes != 1)
      return;

    //get new end_time.
    end_time = sns_ddf_get_timestamp();

    L3GD20_MSG_2(HIGH, "handle_irq: STATUS_REG=%u, FIFO_SRC=%u", rw_buffer[0], rw_buffer[1]);

    //FIFO overflow interrupt handling.
    if((rw_buffer[1] & 0x40) && (fifo_run_cnt==0))
    {
      L3GD20_MSG_2(HIGH, "handle_irq: to send overflow event, watermark=%u, timestamp=%u",
          state->fifo_wmk, timestamp);

      // send a FIFO overflow event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->smgr_hndl,
          SNS_DDF_SENSOR_GYRO,
          SNS_DDF_EVENT_FIFO_OVERFLOW);
      if(status != SNS_DDF_SUCCESS)
        return;

      fifo_run_cnt ++;
      run_again = true;
    }

    //FIFO watermark interrupt(without overflow flag) handling
    if((0x0 == (rw_buffer[1] & 0x40)) && (rw_buffer[1] & 0x80))
    {
      total_in_fifo = rw_buffer[1] & 0x1F;
      L3GD20_MSG_1(HIGH, "handle_irq: WM INT happenes. total_in_fifo=%u", total_in_fifo);

      //if there is no data in FIFO.
      if(0 == total_in_fifo)
        return;

      // send a FIFO Watermark event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->smgr_hndl,
          SNS_DDF_SENSOR_GYRO,
          SNS_DDF_EVENT_FIFO_WM_INT);
      if(status != SNS_DDF_SUCCESS)
        return;

      //calculate start_time and cal_end for three different cases.
      if(0 == fifo_run_cnt)
      {
        if(timestamp > end_time)
          timestamp = end_time;

        //1.if more samples than fifo_wmk for the first fifo irq run, recalcuate the end timestamp of last sample.
        if(total_in_fifo > state->fifo_wmk)
        {
          if((timestamp > state->last_timestamp)&&(0 != state->last_timestamp))
          {
            //use state->last_timestamp as reliable source.
            cal_odr = (end_time - state->last_timestamp) / total_in_fifo;
            if((cal_odr < 0.5*est_odr) || (cal_odr > 1.5*est_odr))
              cal_odr = est_odr;
            start_time = state->last_timestamp + cal_odr;
          }
          else
          {
            //use timestamp as reliable source.
            cal_odr = est_odr;
            start_time = end_time - cal_odr * (total_in_fifo - 1);
          }
          cal_end = end_time;
        }
        //2.if fifo has fifo_wmk for the first fifo irq run
        else
        {
          if((timestamp > state->last_timestamp)&&(0 != state->last_timestamp))
          {
            //use state->last_timestamp as reliable source.
            cal_odr = (timestamp - state->last_timestamp) / total_in_fifo;
            if((cal_odr < 0.5*est_odr) || (cal_odr > 1.5*est_odr))
              cal_odr = est_odr;
            start_time = state->last_timestamp + cal_odr;
          }
          else
          {
            //use timestamp as reliable source.
            cal_odr = est_odr;
            start_time = timestamp - cal_odr * (total_in_fifo - 1);
          }
           cal_end = timestamp;
        }
      }
      else
      {
        //3. read whatever has in FIFO for 2nd irq run. use state->last_timestamp as reliable source.
        start_time = state->last_timestamp + cal_odr;
        cal_end = state->last_timestamp + cal_odr * total_in_fifo;
      }

      
      end_time = cal_end;
      
      //update last_timestamp
      //state->last_timestamp = end_time;

      L3GD20_MSG_3(HIGH, "handle_irq: to call send_fifo_data, total_to_read=%u, starttime=%u, real_endtime=%u",
          total_in_fifo, start_time, end_time);

      // send all data samples in FIFO to SMGR.
      status = sns_dd_gyr_send_fifo_data(state,
          total_in_fifo,
          start_time,
          end_time);
      if(status != SNS_DDF_SUCCESS)
        return;

      fifo_run_cnt ++;
      run_again = true;
    }

    //special case, FIFO DRI interrupt handling for watermark 1.
    if((rw_buffer[0] & 0x08) && (state->fifo_enabled)
        && (state->fifo_int_enabled) && (1==state->fifo_wmk))
    {
      // send a FIFO Watermark event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->smgr_hndl,
          SNS_DDF_SENSOR_GYRO,
          SNS_DDF_EVENT_FIFO_WM_INT);
      if(status != SNS_DDF_SUCCESS)
        return;

      L3GD20_MSG_2(HIGH, "handle_irq: to call send_fifo_data, watermark=%u, timestamp=%u",
          state->fifo_wmk, timestamp);

      //update last_timestamp
      //state->last_timestamp = timestamp;

      status = sns_dd_gyr_send_fifo_data(state,
          state->fifo_wmk,
          timestamp,
          timestamp);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

  }

  L3GD20_MSG_1(HIGH, "handle_irq: DD ISR end time=%u", sns_ddf_get_timestamp());
}


#if STM_L3GD20_LOGDATA
/*===========================================================================
FUNCTION:   sns_dd_gyr_log_data
===========================================================================*/
/*!
  @brief log gyro sensor data

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] gyr_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
static void sns_dd_gyr_log_data(sns_ddf_sensor_data_s *gyr_data_ptr)
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
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_GYRO;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = gyr_data_ptr->timestamp;

    //Log the sensor data
    log_struct_ptr->num_data_types = STM_L3GD20_NUM_AXES;
    log_struct_ptr->data[0]  = gyr_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = gyr_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = gyr_data_ptr->samples[2].sample;

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
        log_struct_ptr);
  }
}

/*===========================================================================
FUNCTION:   sns_dd_gyr_log_fifo
===========================================================================*/
/*!
  @brief log gyr sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] gyr_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
static void sns_dd_gyr_log_fifo(sns_ddf_sensor_data_s *gyr_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  uint16 idx =0 ;

  if( sns_ddf_smgr_is_in_uimage() )
    return;
    
  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s) + (gyr_data_ptr->num_samples -1)*sizeof(int32_t),
      (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_GYRO;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = gyr_data_ptr->timestamp;
    log_struct_ptr->end_timestamp = gyr_data_ptr->end_timestamp;

    log_struct_ptr->num_data_types = STM_L3GD20_NUM_AXES;
    log_struct_ptr->num_samples = gyr_data_ptr->num_samples / STM_L3GD20_NUM_AXES;

    //Log the sensor fifo data
    log_struct_ptr->data[0]  = gyr_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = gyr_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = gyr_data_ptr->samples[2].sample;

    for(idx=0; idx<gyr_data_ptr->num_samples; idx++)
    {
      log_struct_ptr->samples[idx]  = gyr_data_ptr->samples[idx].sample;
    }

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
        log_struct_ptr);
  }
}

#endif


/**
 * @brief Retrieves a single set of angular rate data from l3gd20.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
  uint8_t read_buffer[STM_L3GD20_NUM_READ_BYTES];
  uint8_t i, read_count = 0;
  sns_ddf_time_t timestamp;
  sns_ddf_status_e status;
  sns_ddf_sensor_data_s *data_ptr;
  sns_dd_gyr_l3gd20_state_t *state = dd_handle;
  // sensor coordinate x,y,z axis raw hardware data
  int16_t hw_d[STM_L3GD20_NUM_AXES];
  // SAE coordinate x,y,z axis data
  q16_t sae_d[STM_L3GD20_NUM_AXES];

  //if current power mode is LOWPOWER , return error.
  if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors != 1))
    return SNS_DDF_EDEVICE;

  // This is a synchronous driver, so try to read data now.
  status = sns_ddf_read_port(
      state->port_handle,
      AUTO_INCREMENT | STM_L3GD20_OUT_X_L,
      read_buffer,
      STM_L3GD20_NUM_READ_BYTES,
      &read_count);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(read_count != STM_L3GD20_NUM_READ_BYTES)
    return SNS_DDF_EBUS;

  // get current time stamp
  timestamp = sns_ddf_get_timestamp();

  //convert the raw data in read_buffer to X/Y/Z axis sensor data
  hw_d[0] = (int16_t) (((read_buffer[1]) << 8) | read_buffer[0]);
  hw_d[1] = (int16_t) (((read_buffer[3]) << 8) | read_buffer[2]);
  hw_d[2] = (int16_t) (((read_buffer[5]) << 8) | read_buffer[4]);

  //adjust sensor data per current sensitivity, and convert to Q16
  sae_d[0] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)hw_d[0],0), FX_CONV_Q16((int32_t)1000,0)),
      state->gyr_sstvt);

  sae_d[1] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)hw_d[1],0), FX_CONV_Q16((int32_t)1000,0)),
      state->gyr_sstvt);

  sae_d[2] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)hw_d[2],0), FX_CONV_Q16((int32_t)1000,0)),
      state->gyr_sstvt);

  //map the sensor data to the phone's coordinates
  sns_ddf_map_axes(&state->axes_map, sae_d);

  //allocate memory for sns_ddf_sensor_data_s data structure
  data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
  if(NULL == data_ptr)
    return SNS_DDF_ENOMEM;

  *data = data_ptr;

  data_ptr->sensor = sensors[0];
  data_ptr->timestamp = timestamp;
  data_ptr->status = ((int32_t)(state->odr_settled_ts - timestamp) > 0) ?
                     SNS_DDF_EINVALID_DATA: SNS_DDF_SUCCESS;

  //allocate memory for data samples
  data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
      STM_L3GD20_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
  if(NULL == data_ptr->samples)
    return SNS_DDF_ENOMEM;

  //Fill out sensor output data and status.
  for (i=0; i<STM_L3GD20_NUM_AXES; i++)
  {
    data_ptr->samples[i].sample = sae_d[i];
    data_ptr->samples[i].status = data_ptr->status;
  }
  data_ptr->num_samples = STM_L3GD20_NUM_AXES;

  if (EnableI2C == false)
  {
     static uint32 dummyL3gd20Data = 1;
     data_ptr->samples[0].sample = dummyL3gd20Data++;
     data_ptr->samples[1].sample = dummyL3gd20Data++;
     data_ptr->samples[2].sample = dummyL3gd20Data++;
  }

  L3GD20_MSG_3(HIGH, "L3GD20 get_data=ts(%d), rsvd(%d),rsvd(%d)",
      data_ptr->timestamp, 0, 0);
  L3GD20_MSG_3(HIGH, "L3GD20 get_data=s0(%d), s1(%d), s2(%d)",
      data_ptr->samples[0].sample, data_ptr->samples[1].sample,
      data_ptr->samples[2].sample);

#if STM_L3GD20_LOGDATA
  sns_dd_gyr_log_data(data_ptr);
#endif

  return SNS_DDF_SUCCESS;
}

/**
* @brief Called when the timer set by this driver has expired.This must be
*        the callback function submitted when initializing a timer.
*
*        Right now this timer handler is only used in selftest, where the timer
*        will be initialized
*
* Refer to sns_ddf_driver_if.h for definition.
*/
static void sns_dd_gyr_handle_timer(
    sns_ddf_handle_t dd_handle,
    void* arg)
{
  sns_dd_gyr_l3gd20_state_t* state = (sns_dd_gyr_l3gd20_state_t*)dd_handle;
  sns_ddf_status_e  status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0xA2;
  uint8_t rw_bytes;
  uint32_t error = 0;
  int16_t OUT_NOST[STM_L3GD20_NUM_AXES] = {0};
  int16_t OUT_ST[STM_L3GD20_NUM_AXES] = {0};
  int32_t OUTX_NOST,OUTY_NOST,OUTZ_NOST;
  int32_t OUTX_ST,OUTY_ST,OUTZ_ST;

  if(state->timer == NULL)
  {
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
        SNS_DDF_SENSOR_GYRO, status, error);

    sns_ddf_mfree(arg);

    return;
  }
  else
  {
    sns_ddf_timer_release(state->timer);
    state->timer = NULL;
  }

  //Collect data
  status = sns_dd_gyr_collect_testdata(dd_handle, OUT_NOST, &error, 6);
  if(status != SNS_DDF_SUCCESS)
  {
    L3GD20_MSG_3(HIGH, "L3GD20 HW self test collect test data failed status % d %d %d",status, 0, 0);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
        SNS_DDF_SENSOR_GYRO, status, error);

    sns_ddf_mfree(arg);

    return;
  }

  //Enable self test
  status = sns_ddf_write_port(
      state->port_handle,
      STM_L3GD20_CTRL_REG4,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
  {
    L3GD20_MSG_3(HIGH, "L3GD20 HW self test write port failed status %d %d %d", status, 0,0);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
        SNS_DDF_SENSOR_GYRO, status, error);

    sns_ddf_mfree(arg);

    return;
  }

  if(rw_bytes != 1)
  {
    L3GD20_MSG_3(HIGH, "L3GD20 HW self test write port failed rw bytes %d %d %d",rw_bytes,0,0);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
        SNS_DDF_SENSOR_GYRO, SNS_DDF_EBUS, error);

    sns_ddf_mfree(arg);

    return;
  }

  // wait 60 ms.
  sns_ddf_delay(60000);

  //Collect data after self test is enabled.
  status = sns_dd_gyr_collect_testdata(dd_handle, OUT_ST, &error, 6);
  if(status != SNS_DDF_SUCCESS)
  {
    L3GD20_MSG_3(HIGH, "L3GD20 HW self test collect test data failed status %d %d %d",status, 0,0);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
        SNS_DDF_SENSOR_GYRO, SNS_DDF_EBUS, error);

    sns_ddf_mfree(arg);

    return;
  }

  //convert to milli-dps unit.
  OUTX_NOST = OUT_NOST[0] * 70;
  OUTY_NOST = OUT_NOST[1] * 70;
  OUTZ_NOST = OUT_NOST[2] * 70;
  OUTX_ST = OUT_ST[0] * 70;
  OUTY_ST = OUT_ST[1] * 70;
  OUTZ_ST = OUT_ST[2] * 70;

  if((STM_L3GD20_ST_MIN > abs(OUTX_ST-OUTX_NOST))
      || (abs(OUTX_ST-OUTX_NOST) > STM_L3GD20_ST_MAX)
      ||(STM_L3GD20_ST_MIN > abs(OUTY_ST-OUTY_NOST))
      || (abs(OUTY_ST-OUTY_NOST) > STM_L3GD20_ST_MAX)
      ||(STM_L3GD20_ST_MIN > abs(OUTZ_ST-OUTZ_NOST))
      || (abs(OUTZ_ST-OUTZ_NOST) > STM_L3GD20_ST_MAX))
  {
    status = SNS_DDF_EFAIL;
  }
#ifdef CONFIG_OEM
  int axis_validated = 0x00;
  int axis_bias_validated= 0x00;

  if ((bool *) arg)
  {
    //Check self test limits
    if((STM_L3GD20_ST_MIN > abs(OUTX_ST-OUTX_NOST))
        || (abs(OUTX_ST-OUTX_NOST) > STM_L3GD20_ST_MAX))
      axis_validated |= AXIS_FAIL_Y;

    if((STM_L3GD20_ST_MIN > abs(OUTY_ST-OUTY_NOST))
        || (abs(OUTY_ST-OUTY_NOST) > STM_L3GD20_ST_MAX))
      axis_validated |= AXIS_FAIL_X;

    if((STM_L3GD20_ST_MIN > abs(OUTZ_ST-OUTZ_NOST))
        || (abs(OUTZ_ST-OUTZ_NOST) > STM_L3GD20_ST_MAX))
      axis_validated |= AXIS_FAIL_Z;

    error = (axis_bias_validated << 8 | axis_validated);
    if (error)
    {
      status = SNS_DDF_EFAIL;
    }
    L3GD20_MSG_3(HIGH, "L3GD20 OEM HW self test collect test data failed status %d %d %d", status, error, 0);
  }
#endif

  //Disable sensor and self test
  rw_buffer = 0x0;
  sns_ddf_write_port(
      state->port_handle,
      STM_L3GD20_CTRL_REG1,
      &rw_buffer,
      1,
      &rw_bytes);

  rw_buffer = 0x0;
  sns_ddf_write_port(
      state->port_handle,
      STM_L3GD20_CTRL_REG4,
      &rw_buffer,
      1,
      &rw_bytes);

  L3GD20_MSG_3(HIGH, "L3GD20 HW self test returning status %d %d %d",status, 0, 0);

  sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
      SNS_DDF_SENSOR_GYRO, status, error);

  sns_ddf_mfree(arg);
}

#if  0
sns_ddf_status_e sns_dd_lgd20_reg_dump(sns_dd_gyr_l3gd20_state_t *state)
{
	sns_ddf_status_e stat;
	uint8_t readbyte = 0;
	uint8_t  out;

	if ((stat = sns_ddf_read_port(state->port_handle,
					22,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

  
  L3GD20_MSG_3(ERROR, "sns_dd_lgd20_reg_dump  %d %d %d",
			6666685, out, readbyte);
	return stat;
}
#endif

/**
 * @brief Implement enable_sched_data() DDF API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_enable_sched(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
  sns_dd_gyr_l3gd20_state_t *state = dd_handle;

  L3GD20_MSG_2(HIGH, "enable_sched_data: sensor=%u, enable=%u", sensor, enable);

  //each called function will handle concurrencies when to enable/disable FIFO int.

  switch(sensor)
  {
    case SNS_DDF_SENSOR_GYRO:
      {
        if(enable)
        {
          return sns_dd_gyr_enable_fifo_int(state);

        }else{

          return sns_dd_gyr_disable_fifo_int(state);
        }

        break;
      }
    default:
      return SNS_DDF_EINVALID_ATTR;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Implement trigger_fifo_data API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_trigger_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint16_t num_samples,
    bool trigger_now)
{
  sns_dd_gyr_l3gd20_state_t *state = dd_handle;
  uint8_t total_in_fifo = 0;
  uint8_t total_to_read = 0;
  sns_ddf_time_t start_time, end_time, est_odr;

  L3GD20_MSG_2(HIGH, "trigger_fifo: get called, num_samples=%u, trigger_now=%u",
      num_samples, trigger_now);

  if((SNS_DDF_SENSOR_GYRO != sensor) || (num_samples > STM_L3GD20_MAX_FIFO))
  {
    return SNS_DDF_EINVALID_PARAM;
  }
 
  if(!trigger_now || !state->fifo_enabled)
  {
    return SNS_DDF_SUCCESS;
  }

  total_in_fifo = sns_dd_gyr_get_fifo_sample_count(state);

  //to avoid that there is no sample in FIFO.
  if(0 == total_in_fifo)
  {
    return SNS_DDF_SUCCESS;
  }

  end_time = sns_ddf_get_timestamp();
  est_odr = sns_ddf_convert_usec_to_tick(1000000 / state->gyr_cur_rate);

  L3GD20_MSG_1(HIGH, "trigger_fifo: total_in_fifo=%d", total_in_fifo);

  if(0 == state->last_timestamp)
    state->last_timestamp = end_time - total_in_fifo * est_odr;

  //TODO: check if possible start time is bigger than end_time.
  if(0 == num_samples)
  {
    total_to_read = total_in_fifo;

    if(total_in_fifo<32)
    {
      start_time = state->last_timestamp +
        (end_time - state->last_timestamp)/total_in_fifo;
    }
    else
    {
      start_time = end_time - 31 * est_odr;
    }
  }
  else
  {
    //calculate how many samples driver needs to provide
    total_to_read = MIN(num_samples, total_in_fifo);
    total_to_read = MIN(total_to_read, state->fifo_wmk);

    if(total_in_fifo<32)
    {
      //use state->last_timestamp as more accurate source.
      start_time = state->last_timestamp +
        (end_time - state->last_timestamp)/total_in_fifo;

      //rare case: fifo has more data samples than what SMGR wants,
      //then recalculate end_timestamp
      if(total_to_read < total_in_fifo)
      {
        end_time = state->last_timestamp +
          total_to_read * (end_time - state->last_timestamp)/total_in_fifo;
      }
    }
    else
    {
      //use end_time as more accurate source.
      start_time = end_time - 31 * est_odr;

      //rare case: fifo has more data samples than what SMGR wants,
      //then recalculate end_timestamp
      if(total_to_read < total_in_fifo)
      {
        end_time = start_time + (total_to_read - 1) * est_odr;
      }
    }
  }

  L3GD20_MSG_3(HIGH, "trigger_fifo: to call send_fifo_data, total_to_read=%u, starttime=%u, endtime=%u",
        total_to_read, start_time, end_time);

  //if total_to_read == 1, here start_time will be same as end_time.
  return sns_dd_gyr_send_fifo_data(state, total_to_read, start_time, end_time);
}



